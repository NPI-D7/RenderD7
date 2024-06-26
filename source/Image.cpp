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
#include <renderd7/internal_db.hpp>
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

static void __rd7i_maketex__(C3D_Tex *tex, Tex3DS_SubTexture *sub,
                             std::vector<unsigned char> &buf, int w, int h) {
  if (!tex || !sub) {
    _rd7i_logger()->Write("Invalid Inpit (objects have no adress!)");
    return;
  }
  // RGBA -> Abgr
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int pos = (x + y * w) * 4;
      auto r = buf[pos + 0];
      auto g = buf[pos + 1];
      auto b = buf[pos + 2];
      auto a = buf[pos + 3];
      buf[pos + 0] = a;
      buf[pos + 1] = b;
      buf[pos + 2] = g;
      buf[pos + 3] = r;
    }
  }

  // Pow2
  int wp2 = __rd7i_gp2o__((unsigned int)w);
  int hp2 = __rd7i_gp2o__((unsigned int)h);

  sub->width = (u16)w;
  sub->height = (u16)h;
  sub->left = 0.0f;
  sub->top = 1.0f;
  sub->right = ((float)w / (float)wp2);
  sub->bottom = 1.0 - ((float)h / (float)hp2);

  // Texture Setup
  C3D_TexInit(tex, (u16)wp2, (u16)hp2, GPU_RGBA8);
  C3D_TexSetFilter(tex, GPU_NEAREST, GPU_NEAREST);

  memset(tex->data, 0, tex->size);

  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int dst_pos = ((((y >> 3) * (wp2 >> 3) + (x >> 3)) << 6) +
                     ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                      ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                    4;
      int src_pos = (y * w + x) * 4;

      memcpy(&((unsigned char *)tex->data)[dst_pos], &buf[src_pos], 4);
    }
  }

  C3D_TexFlush(tex);
  tex->border = 0x00000000;
  C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}

namespace RenderD7 {

void Image::Load(const std::string &path) {
  // Make sure to cleanup
  Delete();
  ext = false;
  // Setup Func and Load Data
  int w, h, c = 0;
  unsigned char *image = stbi_load(path.c_str(), &w, &h, &c, 4);
  if (image == nullptr) {
    //_rd7i_logger()->Write("Failed to Load Image: " + path);
    return;
  }
  // Size/Fmt Check
  if (w > 1024 || h > 1024) {
    // Reason: Image to Large
    //_rd7i_logger()->Write("Image too Large!");
    stbi_image_free(image);
    return;
  }

  std::vector<unsigned char> wimg;
  if (c == 3) {
    //_rd7i_logger()->Write("Convert Image to RGBA");
    stbi_image_free(image);
    image = stbi_load(path.c_str(), &w, &h, &c, 3);
    wimg.resize(w * h * 4);
    __rd7i_r24r32(wimg, std::vector<unsigned char>(image, image + (w * h * 3)),
                  w, h);
  } else {
    wimg.assign(&image[0], &image[(w * h * 4) - 1]);
    stbi_image_free(image);
  }
  // Create C2D_Image
  C3D_Tex *tex = new C3D_Tex;
  Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture;
  __rd7i_maketex__(tex, subtex, wimg, w, h);
  _rd7i_logger()->Write(RenderD7::FormatString("Created Texture (%d, %d)",
                                               tex->width, tex->height));
  img = {tex, subtex};
}

void Image::From_NIMG(const nimg &image) {
  // Make sure to cleanup
  Delete();
  ext = false;
  if (image.width > 1024 || image.height > 1024) return;
  C3D_Tex *tex = new C3D_Tex;
  Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture;
  std::vector<unsigned char> mdpb = image.pixel_buffer;
  __rd7i_maketex__(tex, subtex, mdpb, image.width, image.height);
  img = {tex, subtex};
}

C2D_Image Image::Get() {
  if (!Loadet()) {
    _rd7i_logger()->Write("Image not Loadet!");
  }
  return img;
}
C2D_Image &Image::GetRef() {
  if (!Loadet()) {
    _rd7i_logger()->Write("Image not Loadet!");
  }
  return img;
}

void Image::Set(const C2D_Image &i) {
  Delete();
  ext = true;
  img = i;
}

R7Vec2 Image::GetSize() {
  if (!img.subtex) return R7Vec2(0, 0);
  return R7Vec2(img.subtex->width, img.subtex->height);
}

void Image::Delete() {
  if(ext) return;
  if (img.subtex != nullptr) {
    delete img.subtex;
    img.subtex = nullptr;
  }
  if (img.tex != nullptr) {
    C3D_TexDelete(img.tex);
    delete img.tex;
    img.tex = nullptr;
  }
}

bool Image::Loadet() { return (img.subtex != nullptr && img.tex != nullptr); }
}  // namespace RenderD7