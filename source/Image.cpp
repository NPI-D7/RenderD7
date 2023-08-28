#include <renderd7/Image.hpp>
#include <renderd7/ToastsV2.hpp>
#include <renderd7/external/stb_image.h>
extern bool usedbgmsg;

void convert24to32(unsigned char *inputImage, int width, int height,
                   unsigned char *outputImage) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int i = (y * width + x) * 3;    // index of current pixel in input image
      int j = (y * width + x) * 4;    // index of current pixel in output image
      outputImage[j] = inputImage[i]; // copy red channel
      outputImage[j + 1] = inputImage[i + 1]; // copy green channel
      outputImage[j + 2] = inputImage[i + 2]; // copy blue channel
      outputImage[j + 3] = 255; // set alpha channel to 255 (fully opaque)
    }
  }
}

static u32 GetNextPowerOf2(u32 v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return (v >= 64 ? v : 64);
}

static bool C3DTexToC2DImage(C2D_Image *texture, u32 width, u32 height,
                             u8 *buf) {
  if (width >= 1024 || height >= 1024)
    return false;

  C3D_Tex *tex = new C3D_Tex[sizeof(C3D_Tex)];
  Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture[sizeof(Tex3DS_SubTexture)];
  subtex->width = static_cast<u16>(width);
  subtex->height = static_cast<u16>(height);
  // RGBA -> ABGR
  for (u32 row = 0; row < subtex->width; row++) {
    for (u32 col = 0; col < subtex->height; col++) {
      u32 z = (row + col * subtex->width) * 4;

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

  u32 w_pow2 = GetNextPowerOf2(subtex->width);
  u32 h_pow2 = GetNextPowerOf2(subtex->height);
  subtex->left = 0.f;
  subtex->top = 1.f;
  subtex->right = (subtex->width / static_cast<float>(w_pow2));
  subtex->bottom = (1.0 - (subtex->height / static_cast<float>(h_pow2)));
  C3D_TexInit(tex, static_cast<u16>(w_pow2), static_cast<u16>(h_pow2),
              GPU_RGBA8);
  C3D_TexSetFilter(tex, GPU_NEAREST, GPU_NEAREST);

  std::memset(tex->data, 0, tex->size);

  for (u32 x = 0; x < subtex->width; x++) {
    for (u32 y = 0; y < subtex->height; y++) {
      u32 dst_pos = ((((y >> 3) * (w_pow2 >> 3) + (x >> 3)) << 6) +
                     ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                      ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                    4;
      u32 src_pos = (y * subtex->width + x) * 4;
      std::memcpy(&(static_cast<u8 *>(tex->data))[dst_pos],
                  &(static_cast<u8 *>(buf))[src_pos], 4);
    }
  }

  C3D_TexFlush(tex);
  tex->border = RenderD7::Color::Hex("#000000", 0);
  C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
  if (tex && subtex) {
    texture->tex = tex;
    texture->subtex = subtex;
    return true;
  }
  return false;
}

static void OLD_C3DTexToC2DImage(C3D_Tex *tex, Tex3DS_SubTexture *subtex,
                                 u8 *buf, u32 size, u32 width, u32 height,
                                 GPU_TEXCOLOR format) {
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

  u32 w_pow2 = GetNextPowerOf2(width);
  u32 h_pow2 = GetNextPowerOf2(height);

  subtex->width = (u16)width;
  subtex->height = (u16)height;
  subtex->left = 0.0f;
  subtex->top = 1.0f;
  subtex->right = (width / (float)w_pow2);
  subtex->bottom = 1.0 - (height / (float)h_pow2);

  C3D_TexInit(tex, (u16)w_pow2, (u16)h_pow2, format);
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
  linearFree(buf);
}

bool IMG_LoadImageFile(C2D_Image *texture, const char *path) {
  stbi_uc *image = NULL;
  int width = 0, height = 0;
  int nc;

  image = stbi_load(path, &width, &height, &nc, 4);

  if (width > 1024 || height > 1024) {
    stbi_image_free(image);
    return false;
  }

  C3D_Tex *tex = new C3D_Tex;
  Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture;
  OLD_C3DTexToC2DImage(tex, subtex, image, (u32)(width * height * 4),
                       (u32)width, (u32)height, GPU_RGBA8);
  texture->tex = tex;
  texture->subtex = subtex;
  stbi_image_free(image);
  return true;
}

void RenderD7::Image::LoadPng(const std::string path) {
  if (usedbgmsg) {
    // RenderD7::Msg::Display("RenderD7", "Loading Png:" + path, Top);
  }
  std::vector<u8> ImageBuffer;
  unsigned width, height;
  if (loadet) {
    C3D_TexDelete(this->img.tex);
    loadet = false;
  }
  lodepng::decode(ImageBuffer, width, height, path);

  this->img.tex = new C3D_Tex;
  this->img.subtex =
      new Tex3DS_SubTexture({(u16)width, (u16)height, 0.0f, 1.0f,
                             width / 1024.0f, 1.0f - (height / 1024.0f)});

  C3D_TexInit(this->img.tex, 1024, 1024, GPU_RGBA8);
  C3D_TexSetFilter(this->img.tex, GPU_LINEAR, GPU_LINEAR);
  this->img.tex->border = 0xFFFFFFFF;
  C3D_TexSetWrap(this->img.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

  for (u32 x = 0; x < width && x < 1024; x++) {
    for (u32 y = 0; y < height && y < 1024; y++) {
      const u32 dstPos = ((((y >> 3) * (1024 >> 3) + (x >> 3)) << 6) +
                          ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                           ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                         4;

      const u32 srcPos = (y * width + x) * 4;
      ((uint8_t *)this->img.tex->data)[dstPos + 0] =
          ImageBuffer.data()[srcPos + 3];
      ((uint8_t *)this->img.tex->data)[dstPos + 1] =
          ImageBuffer.data()[srcPos + 2];
      ((uint8_t *)this->img.tex->data)[dstPos + 2] =
          ImageBuffer.data()[srcPos + 1];
      ((uint8_t *)this->img.tex->data)[dstPos + 3] =
          ImageBuffer.data()[srcPos + 0];
    }
  }
  loadet = true;
}

RenderD7::Image::~Image() {
  if (loadet)
    C3D_TexDelete(img.tex);
  loadet = false;
}

void RenderD7::Image::Unload() {
  if (loadet)
    C3D_TexDelete(img.tex);
  loadet = false;
}

void RenderD7::Image::LoadPFromBuffer(const std::vector<u8> &buffer) {
  std::vector<u8> ImageBuffer;
  if (loadet) {
    C3D_TexDelete(this->img.tex);
    loadet = false;
  }
  unsigned width, height;
  lodepng::decode(ImageBuffer, width, height, buffer);

  img.tex = new C3D_Tex;
  img.subtex =
      new Tex3DS_SubTexture({(u16)width, (u16)height, 0.0f, 1.0f,
                             width / 512.0f, 1.0f - (height / 512.0f)});

  C3D_TexInit(img.tex, 512, 512, GPU_RGBA8);
  C3D_TexSetFilter(img.tex, GPU_LINEAR, GPU_LINEAR);
  img.tex->border = 0xFFFFFFFF;
  C3D_TexSetWrap(img.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

  for (u32 x = 0; x < width && x < 512; x++) {
    for (u32 y = 0; y < height && y < 512; y++) {
      const u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) +
                          ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                           ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                         4;

      const u32 srcPos = (y * width + x) * 4;
      ((uint8_t *)img.tex->data)[dstPos + 0] = ImageBuffer.data()[srcPos + 3];
      ((uint8_t *)img.tex->data)[dstPos + 1] = ImageBuffer.data()[srcPos + 2];
      ((uint8_t *)img.tex->data)[dstPos + 2] = ImageBuffer.data()[srcPos + 1];
      ((uint8_t *)img.tex->data)[dstPos + 3] = ImageBuffer.data()[srcPos + 0];
    }
  }
}

void RenderD7::Image::FromSheet(RenderD7::Sheet sheet, size_t index) {}

bool RenderD7::Image::Draw(float x, float y, float scaleX, float scaleY) {
  if (loadet)
    return C2D_DrawImageAt(this->img, x, y, 0.5f, nullptr, scaleX, scaleY);
  return false;
}

void RenderD7::Image::LoadFromBitmap(BMP bitmap) {
  loadet = false;
  unsigned error =
      C3DTexToC2DImage(&this->img, bitmap.bmp_info_header.width,
                       bitmap.bmp_info_header.height, bitmap.data.data());
  if (error == 0) {
    this->loadet = true;
  }

  if (error) {
    std::cout << "BMP decoding error " << error << std::endl;
    RenderD7::PushMessage(
        RenderD7::Message("Bmp - Error", "Code: " + std::to_string(error)));
  }
}

namespace RenderD7 {
void Image::LoadJpg(std::string path) {
  if (usedbgmsg) {
    // RenderD7::Msg::Display("RenderD7", "Loading Png:" + path, Top);
  }
  if (loadet) {
    C3D_TexDelete(this->img.tex);
    loadet = false;
  }
  IMG_LoadImageFile(&this->img, path.c_str());
  loadet = true;
}

void Image::LoadPixels(int w, int h, int bpp, void *buffer) {
  if (loadet) {
    C3D_TexDelete(this->img.tex);
    loadet = false;
  }
  unsigned width, height;
  width = w;
  height = h;
  uint8_t *ImageBuffer = new uint8_t[w * h * 4];

  if (bpp == 3)
    convert24to32((uint8_t *)buffer, width, height, ImageBuffer);

  img.tex = new C3D_Tex;
  img.subtex =
      new Tex3DS_SubTexture({(u16)width, (u16)height, 0.0f, 1.0f,
                             width / 512.0f, 1.0f - (height / 512.0f)});

  C3D_TexInit(img.tex, 512, 512, GPU_RGBA8);
  C3D_TexSetFilter(img.tex, GPU_LINEAR, GPU_LINEAR);
  img.tex->border = 0xFFFFFFFF;
  C3D_TexSetWrap(img.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

  for (u32 x = 0; x < width && x < 512; x++) {
    for (u32 y = 0; y < height && y < 512; y++) {
      const u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) +
                          ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                           ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                         4;

      const u32 srcPos = (y * width + x) * 4;
      ((uint8_t *)img.tex->data)[dstPos + 0] = ImageBuffer[srcPos + 3];
      ((uint8_t *)img.tex->data)[dstPos + 1] = ImageBuffer[srcPos + 2];
      ((uint8_t *)img.tex->data)[dstPos + 2] = ImageBuffer[srcPos + 1];
      ((uint8_t *)img.tex->data)[dstPos + 3] = ImageBuffer[srcPos + 0];
    }
  }
  delete[] ImageBuffer;
  loadet = true;
}

} // namespace RenderD7