#ifndef INNSMOUTH_APPLICATION_H
#define INNSMOUTH_APPLICATION_H

#include "gui/include/window.h"

namespace Innsmouth {

class Application {
public:
  Application(std::string_view name, uint32_t width, uint32_t height);

  void Run();

private:
  Window window_;
};

} // namespace Innsmouth

#endif // INNSMOUTH_APPLICATION_H