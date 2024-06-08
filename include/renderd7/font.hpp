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

#include <citro2d.h>

#include <fstream>
#include <memory>
#include <renderd7/Error.hpp>
#include <renderd7/smart_ctor.hpp>

namespace RenderD7 {
class Font {
 public:
  Font() = default;
  Font(const std::string& path) { Load(path); };
  ~Font() { Unload(); }
  RD7_SMART_CTOR(Font)

  void Load(const std::string& path) {
    std::ifstream ft(path, std::ios::in | std::ios::binary);
    bool io = ft.is_open();
    ft.close();
    RenderD7::InlineAssert(io, "File not Found!");
    fnt = C2D_FontLoad(path.c_str());
    RenderD7::InlineAssert(fnt, "Font could not be loaded!");
  }
  C2D_Font Ptr() { return fnt; }
  void Unload() {
    if (!fnt) return;
    C2D_FontFree(fnt);
    fnt = nullptr;
  }

 private:
  C2D_Font fnt = nullptr;
};
}  // namespace RenderD7
