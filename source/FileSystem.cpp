#include <3ds.h>
#include <cstring>
#include <renderd7/FileSystem.hpp>
// Debugging
#include <memory>
#include <renderd7/Ovl.hpp>
#include <renderd7/Toast.hpp>
#include <renderd7/stringtool.hpp>

#include <filesystem>

std::vector<RenderD7::FileSystem::Entry> RenderD7::FileSystem::GetDirContent(std::string path)
{
  std::vector<RenderD7::FileSystem::Entry> res;
  for(const auto& entry : std::filesystem::directory_iterator(std::filesystem::path(path)))
  {
    res.push_back({entry.path().string(), GetFileName(entry.path().string()), entry.is_directory()});
  }
  return res;
}