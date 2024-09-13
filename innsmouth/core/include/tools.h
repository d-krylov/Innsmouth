#ifndef INNSMOUTH_TOOLS_H
#define INNSMOUTH_TOOLS_H

#include <filesystem>
#include <source_location>
#include <vector>

namespace Innsmouth {

[[nodiscard]] double GetTime();
[[nodiscard]] std::vector<std::byte> ReadBinaryFile(const std::filesystem::path &path);

inline const std::filesystem::path ROOT = PROJECT_SOURCE_DIR;
inline const std::filesystem::path SHADER_DIR = ROOT / "shaders" / "spirv";

void INNSMOUTH_LOG(const std::string_view message,
                   const std::source_location location = std::source_location::current());

} // namespace Innsmouth

#endif // INNSMOUTH_TOOLS_H