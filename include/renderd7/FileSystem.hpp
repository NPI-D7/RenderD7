/**
 *   This file is part of RenderD7
 *   Copyright (C) 2021-2024 NPI-D7, tobid7
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
std::vector<Entry> GetDirContentsExt(
    std::string &path, const std::vector<std::string> &extensions);
}  // namespace FileSystem
}  // namespace RenderD7