#ifndef INNSMOUTH_DEPTH_IMAGE_H
#define INNSMOUTH_DEPTH_IMAGE_H

#include "image.h"

namespace Innsmouth {

class DepthImage : public Image {
public:
  DepthImage(uint32_t width, uint32_t height, Format format, SampleCount samples = SampleCount::BIT1);
};

} // namespace Innsmouth

#endif // INNSMOUTH_DEPTH_IMAGE_H