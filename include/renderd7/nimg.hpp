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
