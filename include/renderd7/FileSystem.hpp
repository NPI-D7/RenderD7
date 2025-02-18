#pragma once
#include <string>
#include <vector>

namespace RenderD7 {
namespace FileSystem {
/// @brief A Directory Entry
struct Entry {
  /// @brief Patf of The Entry
  std::string path;
  /// @brief Name of The Entry
  std::string name;
  /// @brief Directory or File
  bool dir = false;
};
/// @brief Gets All Entrys of A Directory into a Vector
/// @param path The Path of the Directory
/// @return The Vector of found Entrys
std::vector<RenderD7::FileSystem::Entry> GetDirContent(std::string path);
std::string GetParentPath(std::string path, std::string mount_point);
std::vector<Entry>
GetDirContentsExt(std::string &path,
                  const std::vector<std::string> &extensions);
} // namespace FileSystem
} // namespace RenderD7