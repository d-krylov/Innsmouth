#ifndef INNSMOUTH_MESH_H
#define INNSMOUTH_MESH_H

#include "graphics/include/buffer.h"
#include "mesh/include/material.h"
#include "mesh/include/vertex.h"
#include <filesystem>
#include <vector>

namespace tinyobj {
class ObjReader;
}

namespace Innsmouth {

class Mesh {
public:
  Mesh();

  void Load(const std::filesystem::path &path);

protected:
  void GetNormal();
  void LoadVertices(const tinyobj::ObjReader &reader);
  void LoadMaterials(const tinyobj::ObjReader &reader);

public:
  std::vector<Vertex> vertices_;
  std::vector<Material> materials_;
  std::vector<std::size_t> offsets_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MESH_H