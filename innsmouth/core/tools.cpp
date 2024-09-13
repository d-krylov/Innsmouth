#include "innsmouth/core/include/tools.h"
#include <fstream>
#include <iostream>

namespace Innsmouth {

auto start_time = std::chrono::high_resolution_clock::now();

double GetTime() {
  auto current_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = current_time - start_time;
  return elapsed.count();
}

std::vector<std::byte> ReadBinaryFile(const std::filesystem::path &path) {
  auto size = std::filesystem::file_size(path);
  std::vector<std::byte> buffer(size);
  std::ifstream input_file(path.string(), std::ios_base::binary);
  input_file.read(reinterpret_cast<char *>(buffer.data()), size);
  input_file.close();
  return buffer;
}

void INNSMOUTH_LOG(const std::string_view message, const std::source_location location) {
  std::clog << "FILE: " << location.file_name() << '(' << location.line() << ':' << location.column() << ") "
            << location.function_name() << " : " << message << '\n';
}

} // namespace Innsmouth