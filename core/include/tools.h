#ifndef INNSMOUTH_TOOLS_H
#define INNSMOUTH_TOOLS_H

#include <filesystem>
#include <vector>

namespace Innsmouth {

[[nodiscard]] double GetTime();
[[nodiscard]] std::vector<std::byte> ReadBinaryFile(const std::filesystem::path &path);
[[nodiscard]] std::filesystem::path GetRoot();

} // namespace Innsmouth

#endif // INNSMOUTH_TOOLS_H