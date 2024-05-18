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

#include <renderd7/external/stb_image.h>

#include <renderd7/Color.hpp>
#include <renderd7/swr.hpp>

namespace RenderD7 {
swr::swr(int w, int h) { image = RenderD7::nimg(w, h); }
swr::swr() { image = RenderD7::nimg(1, 1); }

swr::~swr() {
  // Do nothing
}

void swr::load_file(const std::string& path) {
  int w, h, c;
  uint8_t* dat = stbi_load(path.c_str(), &w, &h, &c, 4);
  image = nimg(w, h);
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int pos = (y * w + x) * 4;
      image.pixel_buffer[pos + 0] = dat[pos + 0];
      image.pixel_buffer[pos + 1] = dat[pos + 1];
      image.pixel_buffer[pos + 2] = dat[pos + 2];
      image.pixel_buffer[pos + 3] = dat[pos + 3];
    }
  }
  stbi_image_free(dat);
}

void swr::load_nimg(const std::string& path) {
  image = RenderD7::NIMG_Load(path);
}

void swr::draw_pixel(int x, int y, unsigned int color) {
  if (x > image.width || x < 0 || y > image.height || y < 0) return;
  RenderD7::Color::RGBA splitter(color);
  image.pixel_buffer[((y * image.width + x) * 4) + 0] = splitter.m_r;
  image.pixel_buffer[((y * image.width + x) * 4) + 1] = splitter.m_g;
  image.pixel_buffer[((y * image.width + x) * 4) + 2] = splitter.m_b;
  image.pixel_buffer[((y * image.width + x) * 4) + 3] = splitter.m_a;
}

void swr::draw_rect(int x, int y, int w, int h, unsigned int color, int t) {
  draw_line(x, y, x + w, y, color, t);
  draw_line(x, y, x, y + h, color, t);
  draw_line(x, y + h, x + w, y + h, color, t);
  draw_line(x + w, y + h, x + w, y + h, color, t);
}

void swr::draw_rect_solid(int x, int y, int w, int h, unsigned int color) {
  for (int ix = x; ix < x + w; ix++) {
    for (int iy = y; iy < y + h; iy++) {
      draw_pixel(ix, iy, color);
    }
  }
}

void swr::draw_line(int x1, int y1, int x2, int y2, unsigned int color, int t) {
  for (int ix = x1; ix < x2 * t; ix++) {
    for (int iy = y1; iy < y2 * t; iy++) {
      draw_pixel(ix, iy, color);
    }
  }
}

void swr::flip(bool h, bool v) {
  const nimg _bak = image;
  if (h) {
    for (int x = 0; x < image.width; x++) {
      for (int y = 0; y < image.height; y++) {
        int src = (y * image.width + x) * 4;
        int dst = ((image.height - 1 - y) * image.width + x) * 4;
        image.pixel_buffer[src + 0] = _bak.pixel_buffer[dst + 3];
        image.pixel_buffer[src + 1] = _bak.pixel_buffer[dst + 2];
        image.pixel_buffer[src + 2] = _bak.pixel_buffer[dst + 1];
        image.pixel_buffer[src + 3] = _bak.pixel_buffer[dst + 0];
      }
    }
  }

  if (v) {
    for (int x = 0; x < image.width; x++) {
      for (int y = 0; y < image.height; y++) {
        int src = (y * image.width + x) * 4;
        int dst = (y * image.width + (image.width - 1 - x)) * 4;
        image.pixel_buffer[src + 0] = _bak.pixel_buffer[dst + 3];
        image.pixel_buffer[src + 1] = _bak.pixel_buffer[dst + 2];
        image.pixel_buffer[src + 2] = _bak.pixel_buffer[dst + 1];
        image.pixel_buffer[src + 3] = _bak.pixel_buffer[dst + 0];
      }
    }
  }
}
}  // namespace RenderD7