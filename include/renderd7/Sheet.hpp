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
#include <3ds.h>  // Result
#include <citro2d.h>
#include <citro3d.h>

#include <string>

namespace RenderD7 {
/// @brief SpriteSheet Class
class Sheet {
 public:
  /// @brief Constructor
  Sheet() = default;
  /// @brief Deconstructor
  ~Sheet() {
    if (spritesheet) Free();
  }
  /// @brief Load A Spritesheet File
  /// @param path Path to the t3x
  /// @return Result Code
  Result Load(const std::string& path);
  /// @brief Unload the Sheet
  void Free();
  /// \param spritesheet The Sheet
  C2D_SpriteSheet spritesheet;
};
}  // namespace RenderD7