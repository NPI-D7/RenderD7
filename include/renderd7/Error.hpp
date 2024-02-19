#pragma once

#include <string>

namespace RenderD7 {
void Error(const std::string& msg);
inline void InlineError(const std::string& msg) {
  std::string location = __FILE__ + std::string(":") + std::to_string(__LINE__);
  Error("Error: \n" + location + "\n" + msg);
}
inline void InlineAssert(bool v, const std::string& msg) {
  if (v == false) {
    std::string location =
        __FILE__ + std::string(":") + std::to_string(__LINE__);
    Error("Assert Failed:\n" + location + "\n" + msg);
  }
}
}  // namespace RenderD7