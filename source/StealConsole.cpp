#include <iostream>
#include <renderd7/StealConsole.hpp>

namespace RenderD7 {
StealConsole::StealConsole() {
  std::streambuf *old = std::cout.rdbuf(this->stolen_stdout.rdbuf());
  if (old) {
    // To prevent from unused error
  }
}

StealConsole::~StealConsole() {
  // Do Nothing Here
}

std::string StealConsole::GetStdout() { return this->stolen_stdout.str(); }
} // namespace RenderD7