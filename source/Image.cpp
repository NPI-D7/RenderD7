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

#include <renderd7/Image.hpp>
#include <vector>

static u32 __rd7i_gp2o__(u32 v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return (v >= 64 ? v : 64);
}

static void __rd7i_r24r32(std::vector<uint8_t> &out,
                          const std::vector<uint8_t> &in, const int &w,
                          const int &h) {
  // Converts RGB24 to RGBA32
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int src = (y * w + x) * 3;
      int dst = (y * w + x) * 4;
      out[dst + 0] = in[src + 0];
      out[dst + 1] = in[src + 1];
      out[dst + 2] = in[src + 2];
      out[dst + 3] = 255;
    }
  }
}

static void __rd7i_c3dc2d__(C3D_Tex *tex, Tex3DS_SubTexture *subtex, u8 *buf,
                            u32 size, u32 width, u32 height) {
  // RGBA -> ABGR
  for (u32 row = 0; row < width; row++) {
    for (u32 col = 0; col < height; col++) {
      u32 z = (row + col * width) * 4;

      u8 r = *(u8 *)(buf + z);
      u8 g = *(u8 *)(buf + z + 1);
      u8 b = *(u8 *)(buf + z + 2);
      u8 a = *(u8 *)(buf + z + 3);

      *(buf + z) = a;
      *(buf + z + 1) = b;
      *(buf + z + 2) = g;
      *(buf + z + 3) = r;
    }
  }

  u32 w_pow2 = __rd7i_gp2o__(width);
  u32 h_pow2 = __rd7i_gp2o__(height);

  subtex->width = (u16)width;
  subtex->height = (u16)height;
  subtex->left = 0.0f;
  subtex->top = 1.0f;
  subtex->right = (width / (float)w_pow2);
  subtex->bottom = 1.0 - (height / (float)h_pow2);

  C3D_TexInit(tex, (u16)w_pow2, (u16)h_pow2, GPU_RGBA8);
  C3D_TexSetFilter(tex, GPU_NEAREST, GPU_NEAREST);

  u32 pixel_size = (size / width / height);

  memset(tex->data, 0, tex->size);

  for (u32 x = 0; x < width; x++) {
    for (u32 y = 0; y < height; y++) {
      u32 dst_pos = ((((y >> 3) * (w_pow2 >> 3) + (x >> 3)) << 6) +
                     ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                      ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                    pixel_size;
      u32 src_pos = (y * width + x) * pixel_size;

      memcpy(&((u8 *)tex->data)[dst_pos], &((u8 *)buf)[src_pos], pixel_size);
    }
  }

  C3D_TexFlush(tex);

  tex->border = 0x00000000;
  C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
  // linearFree(buf);
}

namespace RenderD7 {
Image::Image() {
  img.tex = nullptr;
  img.subtex = nullptr;
}

Image::~Image() { safe_del(); }

void Image::load(const std::string &path) {
  // Make sure to cleanup
  safe_del();
  ld = false;
  // Setup Func and Load Data
  int w, h, c = 0;
  uint8_t *image = stbi_load(path.c_str(), &w, &h, &c, 4);
  if (image == nullptr) {
    return;
  }
  // Size/Fmt Check
  if (w > 1024 || h > 1024) {
    // Reason: Image to Large
    stbi_image_free(image);
    return;
  }

  std::vector<uint8_t> wimg(w * h * 4);
  if (c == 3) {
    stbi_image_free(image);
    image = stbi_load(path.c_str(), &w, &h, &c, 3);
    __rd7i_r24r32(wimg, std::vector<uint8_t>(image, image + (w * h * 3)), w, h);
  } else {
    // Maybe find a better solution later
    for (size_t i = 0; i < wimg.size(); i++) {
      wimg[i] = image[i];
    }
  }
  stbi_image_free(image);
  // Create C2D_Image
  C3D_Tex *tex = new C3D_Tex;
  Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture;
  __rd7i_c3dc2d__(tex, subtex, wimg.data(), (u32)(w * h * 4), (u32)w, (u32)h);
  img.tex = tex;
  img.subtex = subtex;
  ld = true;
}

void Image::from_nimg(const nimg &image) {
  // Make sure to cleanup
  safe_del();
  ld = false;
  if (image.width > 1024 || image.height > 1024) return;
  C3D_Tex *tex = new C3D_Tex;
  Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture;
  __rd7i_c3dc2d__(tex, subtex, (u8 *)image.pixel_buffer.data(),
                  (u32)image.pixel_buffer.size(), (u32)image.width,
                  (u32)image.height);
  img.tex = tex;
  img.subtex = subtex;
  ld = true;
}

C2D_Image Image::get() { return img; }
C2D_Image &Image::get_ref() { return img; }

void Image::set(const C2D_Image &i) {
  safe_del();
  img = i;
}

R7Vec2 Image::get_size() {
  if (!img.subtex) return R7Vec2(0, 0);
  return R7Vec2(img.subtex->width, img.subtex->height);
}

void Image::safe_del() {
  if (img.subtex != nullptr) delete img.subtex;
  if (img.tex != nullptr) delete img.tex;
}

bool Image::loaded() { return ld; }
}  // namespace RenderD7