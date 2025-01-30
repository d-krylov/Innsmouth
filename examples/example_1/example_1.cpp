#include "graphics/include/graphics.h"
#include "graphics/pipeline/shader_module.h"

using namespace Innsmouth;

int main() {

  // CreateGraphics(DebugOptions(DebugMessageType::GENERAL, DebugMessageSeverity::VERBOSE));

  ShaderModule s("../shaders/spirv/gui.vert.spv");

  return 0;
}