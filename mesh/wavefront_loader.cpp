#include "easyloggingpp/easylogging++.h"
#include "mesh/include/mesh.h"
#include "mesh/include/vertex.h"
#include "tinyobjloader/tiny_obj_loader.h"

namespace Innsmouth {

void Mesh::LoadVertices(const tinyobj::ObjReader &reader) {
  auto &shapes = reader.GetShapes();
  auto &attributes = reader.GetAttrib();
  for (const auto &shape : shapes) {
    std::size_t index_offset = 0;
    offsets_.emplace_back(vertices_.size());
    material_indices_.emplace_back(shape.mesh.material_ids[0]);

    for (std::size_t i = 0; i < shape.mesh.num_face_vertices.size(); i++) {
      auto vertices_in_face = shape.mesh.num_face_vertices[i];

      for (std::size_t v = 0; v < vertices_in_face; v++) {
        auto index = shape.mesh.indices[index_offset + v];
        Vertex vertex;
        auto vx = attributes.vertices[3 * index.vertex_index + 0];
        auto vy = attributes.vertices[3 * index.vertex_index + 1];
        auto vz = attributes.vertices[3 * index.vertex_index + 2];
        vertex.position_ = Vector3f(vx, vy, vz);

        if (attributes.normals.empty() == false) {
          auto nx = attributes.normals[3 * index.normal_index + 0];
          auto ny = attributes.normals[3 * index.normal_index + 1];
          auto nz = attributes.normals[3 * index.normal_index + 2];
          vertex.normal_ = Vector3f(nx, ny, nz);
        }

        if (index.texcoord_index >= 0) {
          auto uvx = attributes.texcoords[2 * index.texcoord_index + 0];
          auto uvy = attributes.texcoords[2 * index.texcoord_index + 1];
          vertex.uv_ = Vector2f(uvx, uvy);
        }
        vertices_.emplace_back(vertex);
      }
      index_offset += vertices_in_face;
    }
  }
}

void Mesh::LoadMaterials(const tinyobj::ObjReader &reader) {
  auto &materials = reader.GetMaterials();

  for (auto &m : materials) {
    materials_.emplace_back();
    auto &material = materials_.back();

    MaterialProperties material_properties{
      .ambient_ = Vector3f(m.ambient[0], m.ambient[1], m.ambient[2]),
      .diffuse_ = Vector3f(m.diffuse[0], m.diffuse[1], m.diffuse[2]),
      .specular_ = Vector3f(m.specular[0], m.specular[1], m.specular[2]),
      .transmittance_ = Vector3f(m.transmittance[0], m.transmittance[1], m.transmittance[2]),
      .emission_ = Vector3f(m.emission[0], m.emission[1], m.emission[2]),
      .shininess_ = m.shininess,
      .ior_ = m.ior,
      .dissolve_ = m.dissolve};

    material.properties_ = std::make_shared<MaterialProperties>(material_properties);

    if (m.ambient_texname.empty() == false) {
      auto image_path = path_.parent_path() / m.ambient_texname;
      CoreImage ambient(image_path);
      material.ambient_texture_ = std::make_shared<Image2D>(ambient);
      LOG(INFO) << m.ambient_texname;
    }

    if (m.diffuse_texname.empty() == false) {
      auto image_path = path_.parent_path() / m.diffuse_texname;
      CoreImage diffuse(image_path);
      material.diffuse_texture_ = std::make_shared<Image2D>(diffuse);
      LOG(INFO) << m.diffuse_texname;
    }

    if (m.specular_texname.empty() == false) {
    }
  }
}

void Mesh::Load(const std::filesystem::path &path) {
  tinyobj::ObjReader reader;
  tinyobj::ObjReaderConfig reader_config;

  path_ = path;

  auto status = reader.ParseFromFile(path.string(), reader_config);

  if (status != true) {
    LOG(FATAL) << "Fail model loading";
  }

  LoadVertices(reader);
  LoadMaterials(reader);

  GetNormal();
}

} // namespace Innsmouth