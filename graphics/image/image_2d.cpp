#include "image_2d.h"
#include "graphics/include/buffer.h"
#include "graphics/include/command_buffer.h"
#include "graphics/include/graphics.h"

namespace Innsmouth {

Image2D::Image2D() : channels_(4) {}

void Image2D::SetData(std::span<std::byte> data, uint32_t layers, uint32_t base_layer) {}

} // namespace Innsmouth