#include "application/include/application.h"

namespace Innsmouth {

Application::Application(std::string_view name, uint32_t width, uint32_t height) : window_(name, width, height) {}

void Application::Run() {
  while (window_.ShouldClose() == false) {
  }
}

} // namespace Innsmouth