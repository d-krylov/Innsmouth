#ifndef INNSMOUTH_DEPTH_IMAGE_H
#define INNSMOUTH_DEPTH_IMAGE_H

#include "image.h"

namespace Innsmouth {

class DepthImage : public Image {
public:
  DepthImage(const VkExtent2D &extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

private:
};

} // namespace Innsmouth

#endif // INNSMOUTH_DEPTH_IMAGE_H