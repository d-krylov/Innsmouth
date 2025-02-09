#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "mesh/include/model.h"
#include <iostream>

namespace Innsmouth {

std::array texture_name_pointers{
  &tinyobj::material_t::ambient_texname, &tinyobj::material_t::diffuse_texname,  &tinyobj::material_t::specular_texname,
  &tinyobj::material_t::bump_texname,    &tinyobj::material_t::metallic_texname, &tinyobj::material_t::roughness_texname,
  &tinyobj::material_t::sheen_texname,   &tinyobj::material_t::emissive_texname, &tinyobj::material_t::normal_texname};

template <IsVertex T> std::vector<T> LoadWavefront(std::span<const tinyobj::shape_t> shapes, const tinyobj::attrib_t &attributes) {
  std::vector<T> vertices;
  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      auto &new_vertex = vertices.emplace_back();
      new_vertex.position_.x = attributes.vertices[3 * index.vertex_index + 0];
      new_vertex.position_.y = attributes.vertices[3 * index.vertex_index + 1];
      new_vertex.position_.z = attributes.vertices[3 * index.vertex_index + 2];
      if (attributes.normals.empty() == false) {
        new_vertex.normal_.x = attributes.normals[3 * index.normal_index + 0];
        new_vertex.normal_.y = attributes.normals[3 * index.normal_index + 1];
        new_vertex.normal_.z = attributes.normals[3 * index.normal_index + 2];
      }
      if constexpr (IsTexturedVertex<T>) {
        if (index.texcoord_index >= 0) {
          auto uvx = attributes.texcoords[2 * index.texcoord_index + 0];
          auto uvy = attributes.texcoords[2 * index.texcoord_index + 1];
          new_vertex.uv_ = Vector2f(uvx, uvy);
        }
      }
    }
  }
  return vertices;
}

void SetMeshMaterial(const tinyobj::ObjReader &reader, const tinyobj::shape_t &shape, Mesh &mesh) {
  auto &materials = reader.GetMaterials();
  if (materials.size() > 0) {
    auto &material = materials[shape.mesh.material_ids[0]];
    for (uint32_t i = 0; i < texture_name_pointers.size(); i++) {
      const auto &texture_name = material.*texture_name_pointers[i];
      if (texture_name.empty() == false) {
        mesh.material_.texture_names_[i] = texture_name;
      }
    }
  }
}

std::vector<Mesh> LoadMeshes(const tinyobj::ObjReader &reader) {
  auto mesh_offset{0};
  auto &shapes = reader.GetShapes();
  std::vector<Mesh> meshes;
  for (const auto &shape : shapes) {
    auto &mesh = meshes.emplace_back();
    mesh.offset_ = mesh_offset;
    mesh.size_ = shape.mesh.indices.size();
    mesh_offset += mesh.size_;
    SetMeshMaterial(reader, shape, mesh);
  }
  return meshes;
}

std::map<std::string, Image2D> LoadTextures(const std::filesystem::path &image_root, std::span<const tinyobj::material_t> materials) {
  std::vector<std::byte> empty(4, std::byte(0));
  std::map<std::string, Image2D> textures{{"default", Image2D(1, 1, empty)}};

  for (const auto &material : materials) {
    for (const auto &texture_name_pointer : texture_name_pointers) {
      const auto &texture_name = material.*texture_name_pointer;
      if (texture_name.size() > 0) {
        if (textures.contains(texture_name) == false) {
          textures.emplace(texture_name, Image2D(image_root / texture_name));
        }
      }
    }
  }

  return textures;
}

std::vector<PlainVertex> LoadWavefrontPlainMesh(const std::filesystem::path &path) {
  tinyobj::ObjReader reader;
  tinyobj::ObjReaderConfig reader_config;
  auto status = reader.ParseFromFile(path.string(), reader_config);
  auto &shapes = reader.GetShapes();
  auto &attributes = reader.GetAttrib();
  auto vertices = LoadWavefront<PlainVertex>(shapes, attributes);
  return vertices;
}

Model LoadWavefrontModel(const std::filesystem::path &path) {
  tinyobj::ObjReader reader;
  tinyobj::ObjReaderConfig reader_config;
  auto status = reader.ParseFromFile(path.string(), reader_config);
  auto &shapes = reader.GetShapes();
  auto &attributes = reader.GetAttrib();
  auto &materials = reader.GetMaterials();
  auto vertices = LoadWavefront<TexturedVertex>(shapes, attributes);
  auto textures = LoadTextures(path.parent_path(), materials);
  auto meshes = LoadMeshes(reader);
  return Model{meshes, vertices, textures};
}

} // namespace Innsmouth