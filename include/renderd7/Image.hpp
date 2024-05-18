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

#include <3ds.h>
#include <citro2d.h>

#include <renderd7/R7Vec.hpp>
#include <renderd7/nimg.hpp>
#include <string>

namespace RenderD7 {
class Image {
 public:
  Image();
  ~Image();
  void load(const std::string& path);
  void from_nimg(const nimg& image);

  C2D_Image get();
  C2D_Image& get_ref();
  void set(const C2D_Image& i);
  R7Vec2 get_size();
  bool loaded();

 private:
  void safe_del();
  bool ld = false;
  C2D_Image img;
};
}  // namespace RenderD7