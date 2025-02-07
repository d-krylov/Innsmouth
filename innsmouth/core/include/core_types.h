#ifndef INNSMOUTH_CORE_TYPES_H
#define INNSMOUTH_CORE_TYPES_H

#include <filesystem>

namespace Innsmouth {

struct Extent2i {
  int32_t width;
  int32_t height;
};

constexpr inline std::size_t operator""_KiB(unsigned long long int x) { return 1024ULL * x; }
constexpr inline std::size_t operator""_MiB(unsigned long long int x) { return 1024_KiB * x; }

inline const std::filesystem::path INNSMOUTH_PROJECT_DIR = INNSMOUTH_ROOT;
inline const std::filesystem::path INNSMOUTH_SHADERS_DIR = INNSMOUTH_PROJECT_DIR / "shaders" / "spirv";

} // namespace Innsmouth

#endif // INNSMOUTH_CORE_TYPES_H