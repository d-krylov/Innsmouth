#include "innsmouth/core/include/core.h"
#include "config/innsmouth_config.h"
#include <print>

namespace Innsmouth {

void CORE_ASSERT(bool status, std::string_view message, std::source_location location) {
  if (status == false) {
    std::println("ERROR: {0} in {1}:{2}", message, location.file_name(), location.line());
    std::abort();
  }
}

std::filesystem::path GetInnsmouthShadersDirectory() {
  return INNSMOUH_PROJECT_BINARY_DIR / "spirv";
}

} // namespace Innsmouth