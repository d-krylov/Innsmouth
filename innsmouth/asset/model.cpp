#include "innsmouth/asset/include/model.h"

namespace Innsmouth {

Model::Model(const std::filesystem::path &path) {
  LoadKhronos(path);
}

std::size_t Model::GetVerticesNumber() const {
  return vertices_.size();
}

std::size_t Model::GetIndicesNumber() const {
  return indices_.size();
}

std::span<const Vertex> Model::GetVertices() const {
  return vertices_;
}

std::span<const uint32_t> Model::GetIndices() const {
  return indices_;
}

std::span<const Mesh> Model::GetMeshes() const {
  return meshes_;
}

std::span<const Image2D> Model::GetImages() const {
  return images_;
}

} // namespace Innsmouth