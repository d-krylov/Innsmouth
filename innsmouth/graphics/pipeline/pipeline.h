#ifndef INNSMOUTH_PIPELINE_H
#define INNSMOUTH_PIPELINE_H

#include "innsmouth/graphics/include/graphics_types.h"

namespace Innsmouth {

struct VertexDescription {
  std::vector<VkVertexInputAttributeDescription> attributes;
  std::vector<VkVertexInputBindingDescription> bindings;
};

class ShaderModule;

class Pipeline {

protected:
protected:
};

} // namespace Innsmouth

#endif // INNSMOUTH_PIPELINE_H