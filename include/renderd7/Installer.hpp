#pragma once

#include <string>

namespace RenderD7 {
struct InstallerInfo {
  unsigned long long total;
  unsigned long long current;
  unsigned int mem_size = 0x80000;
  bool active = false;
};
Result InstallCia(const std::string& path, bool self);
void InstallSetBuffersSize(unsigned int bytes);
InstallerInfo InstallGetInfo();
}  // namespace RenderD7