#include "fastgltf/glm_element_traits.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/tools.hpp"
#include "innsmouth/asset/include/model.h"

namespace Innsmouth {

namespace fgf = fastgltf;

void GetModelProperties(const fgf::Asset &asset, std::size_t &vertices_count, std::size_t &indices_count) {
  for (const auto &mesh : asset.meshes) {
    for (const auto &primitive : mesh.primitives) {
      const auto position_attribute = primitive.findAttribute("POSITION");
      const auto &position_accessor = asset.accessors[position_attribute->accessorIndex];
      const auto &indices_accessor = asset.accessors[primitive.indicesAccessor.value()];
      vertices_count += position_accessor.count;
      indices_count += indices_accessor.count;
    }
  }
}

auto LoadIndices(const fgf::Asset &asset, std::size_t accessor_index, std::size_t ioffset, std::size_t voffset, std::span<uint32_t> out) {
  const auto &indices_accessor = asset.accessors[accessor_index];
  auto indices_count = indices_accessor.count;
  auto indices_begin = out.begin() + ioffset;
  fgf::iterateAccessor<uint32_t>(asset, indices_accessor, [&](uint32_t index) { *indices_begin++ = index + voffset; });
  return ioffset + indices_count;
}

template <typename T, typename Function>
void LoadVertexData(const fgf::Asset &asset, const fgf::Primitive &primitive, std::string_view attribute_name, Function &&callback) {
  if (const auto &attribute = primitive.findAttribute(attribute_name); attribute != primitive.attributes.end()) {
    const auto &accessor = asset.accessors[attribute->accessorIndex];
    fgf::iterateAccessorWithIndex<T>(asset, accessor, std::forward<Function>(callback));
  }
}

auto LoadVertices(const fgf::Asset &asset, const fgf::Primitive &primitive, std::size_t vertices_offset, std::span<Vertex> out_vertices) {
  auto position_attribute = primitive.findAttribute("POSITION");
  const auto &position_accessor = asset.accessors[position_attribute->accessorIndex];
  auto set_position = [&](const Vector3f &position, std::size_t index) { out_vertices[vertices_offset + index].position_ = position; };
  auto set_normal = [&](const Vector3f &normal, std::size_t index) { out_vertices[vertices_offset + index].normal_ = normal; };
  fgf::iterateAccessorWithIndex<Vector3f>(asset, position_accessor, set_position);
  LoadVertexData<Vector3f>(asset, primitive, "NORMAL", set_normal);
  return vertices_offset + position_accessor.count;
}

void LoadPrimitives(const fgf::Asset &asset, std::span<Vertex> out_vertices, std::span<uint32_t> out_indices) {
  std::size_t vertices_offset = 0, indices_offset = 0;
  for (const auto &mesh : asset.meshes) {
    for (auto &primitive : mesh.primitives) {
      auto indices_accessor_index = primitive.indicesAccessor;
      indices_offset = LoadIndices(asset, primitive.indicesAccessor.value(), indices_offset, vertices_offset, out_indices);
      vertices_offset = LoadVertices(asset, primitive, vertices_offset, out_vertices);
    }
  }
}

void LoadImages(const fgf::Asset &asset, const std::filesystem::path &path) {
  for (const auto &image : asset.images) {
    auto image_name = std::get<fastgltf::sources::URI>(image.data).uri.path();
    auto image_path = path.parent_path() / image_name;
  }
}

void Model::LoadKhronos(const std::filesystem::path &path) {
  auto extensions = fgf::Extensions::KHR_mesh_quantization | fgf::Extensions::KHR_texture_transform | fgf::Extensions::KHR_materials_variants;
  auto options = fgf::Options::DontRequireValidAssetMember | fgf::Options::LoadExternalBuffers | fgf::Options::GenerateMeshIndices;

  fastgltf::Parser parser(extensions);

  auto gltf_file = fastgltf::MappedGltfFile::FromPath(path);
  auto asset = parser.loadGltf(gltf_file.get(), path.parent_path(), options);

  std::size_t vertices_count = 0, indices_count = 0;
  GetModelProperties(asset.get(), vertices_count, indices_count);

  vertices_.resize(vertices_count);
  indices_.resize(indices_count);

  LoadPrimitives(asset.get(), vertices_, indices_);
}

} // namespace Innsmouth