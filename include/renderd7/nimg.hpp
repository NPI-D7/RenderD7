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

#include <cstdint>
#include <string>
#include <vector>

#define NPI_NIMG_ (uint32_t)0x4e494d47  // Magic: NIMG

namespace RenderD7 {
struct nimg {
  unsigned int magic;  // Magic number defaults do NPI_NIMG_
  int width;
  int height;
  int format;
  int compression;
  std::vector<unsigned char> pixel_buffer;

  nimg(int w = 0, int h = 0, int fmt = 0, int cmp = 0) {
    magic = NPI_NIMG_;
    width = w;
    height = h;
    format = fmt;
    compression = cmp;
    pixel_buffer.resize((w * h) * (format ? 3 : 4));
  }
};
nimg NIMG_Load(std::string path);
nimg NIMG_LoadFromMem(unsigned char* buffer, size_t bf_size);
void NIMG_Save(nimg image, std::string path);
}  // namespace RenderD7
