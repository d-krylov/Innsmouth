#ifndef INNSMOUTH_COMPUTE_PIPELINE_H
#define INNSMOUTH_COMPUTE_PIPELINE_H

#include "graphics/include/graphics_types.h"
#include <filesystem>

namespace Innsmouth {

class ComputePipeline {
public:
  ComputePipeline(const std::filesystem::path &path);

  ~ComputePipeline();

private:
};

} // namespace Innsmouth

#endif // INNSMOUTH_COMPUTE_PIPELINE_H