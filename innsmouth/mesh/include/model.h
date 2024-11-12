#ifndef INNSMOUTH_MODEL_H
#define INNSMOUTH_MODEL_H

#include "graphics/image/image_2d.h"
#include "material.h"
#include "vertex.h"
#include <filesystem>
#include <map>
#include <vector>

namespace Innsmouth {

struct Mesh {
  uint64_t vertices_offset_;
  uint64_t vertices_size_;
  uint64_t indices_offset_;
  uint64_t indices_size_;
  Material material_;
};

class Model {
public:
  void LoadAssimp(const std::filesystem::path &path);
  void LoadWavefront(const std::filesystem::path &path);

public:
  std::vector<Mesh> meshes_;
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
  std::map<std::string, Image2D> textures_;
  std::filesystem::path path_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MESH_H