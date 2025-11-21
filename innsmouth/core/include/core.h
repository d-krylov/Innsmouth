#ifndef INNSMOUTH_CORE_H
#define INNSMOUTH_CORE_H

#include <cstdint>
#include <filesystem>
#include <source_location>

namespace Innsmouth {

struct ViewportSize {
  int32_t width;
  int32_t height;
};

std::filesystem::path GetInnsmouthShadersDirectory();

void CORE_ASSERT(bool status, std::string_view message, std::source_location location = std::source_location::current());

constexpr inline std::size_t operator""_KiB(unsigned long long int x) {
  return 1024ULL * x;
}

constexpr inline std::size_t operator""_MiB(unsigned long long int x) {
  return 1024_KiB * x;
}

} // namespace Innsmouth

#endif // INNSMOUTH_CORE_H