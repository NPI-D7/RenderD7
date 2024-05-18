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

#include <renderd7/nimg.hpp>

namespace RenderD7 {
class swr {
 public:
  swr(int w, int h);
  swr();
  ~swr();
  nimg& get_image() { return image; }
  void load_file(const std::string& path);
  void load_nimg(const std::string& path);

  // Rendering
  void draw_pixel(int x, int y, unsigned int color);
  void draw_rect(int x, int y, int w, int h, unsigned int color, int t = 1);
  void draw_rect_solid(int x, int y, int w, int h, unsigned int color);
  void draw_line(int x1, int y1, int x2, int y2, unsigned int color, int t = 1);
  void flip(bool h, bool v);

 private:
  nimg image;
};
}  // namespace RenderD7