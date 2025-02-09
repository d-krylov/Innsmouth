#ifndef INNSMOUTH_MODEL_H
#define INNSMOUTH_MODEL_H

#include "mesh.h"
#include "graphics/image/image2D.h"
#include <filesystem>
#include <map>

namespace Innsmouth {

class Model {
public:
  std::vector<Mesh> meshes_;
  std::vector<TexturedVertex> vertices_;
  std::map<std::string, Image2D> textures_;
};

Model LoadWavefrontModel(const std::filesystem::path &path);

} // namespace Innsmouth

#endif // INNSMOUTH_MODEL_H
