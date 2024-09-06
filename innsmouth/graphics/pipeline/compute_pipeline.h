#ifndef INNSMOUTH_COMPUTE_PIPELINE_H
#define INNSMOUTH_COMPUTE_PIPELINE_H

#include "pipeline.h"

namespace Innsmouth {

class ComputePipeline : public Pipeline {
public:
  ComputePipeline(const std::filesystem::path &path);

  ~ComputePipeline();

private:
};

} // namespace Innsmouth

#endif // INNSMOUTH_COMPUTE_PIPELINE_H