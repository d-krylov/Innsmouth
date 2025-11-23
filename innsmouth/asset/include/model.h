#ifndef INNSMOUTH_MODEL_H
#define INNSMOUTH_MODEL_H

#include "mesh.h"
#include "innsmouth/graphics/image/image2D.h"
#include <span>
#include <filesystem>

namespace Innsmouth {

class Model {
public:
  Model() = default;

  Model(const std::filesystem::path &path);

  std::size_t GetVerticesNumber() const;
  std::size_t GetIndicesNumber() const;

  std::span<const Vertex> GetVertices() const;
  std::span<const uint32_t> GetIndices() const;
  std::span<const Mesh> GetMeshes() const;
  std::span<const Image2D> GetImages() const;

protected:
  void LoadKhronos(const std::filesystem::path &path);

private:
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
  std::vector<Mesh> meshes_;
  std::vector<Image2D> images_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_MODEL_H