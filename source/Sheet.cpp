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

#include <renderd7/Sheet.hpp>
#include <renderd7/internal_db.hpp>

Result RenderD7::Sheet::Load(const std::string& path) {
  this->spritesheet = C2D_SpriteSheetLoad(path.c_str());
  if (!this->spritesheet) {
    _rd7i_logger()->Write("Failed to Load Spritesheet from: " + path, 0);
  }
  return 0;
}

void RenderD7::Sheet::Free() {
  if (!this->spritesheet) return;
  C2D_SpriteSheetFree(this->spritesheet);
  this->spritesheet = nullptr;
}
