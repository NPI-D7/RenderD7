#include <renderd7/external/stb_image.h>

#include <renderd7/Color.hpp>
#include <renderd7/nimg_engine.hpp>

namespace RenderD7 {
NIMG_Engine::NIMG_Engine(int w, int h) { image = RenderD7::nimg(w, h); }
NIMG_Engine::NIMG_Engine() { image = RenderD7::nimg(1, 1); }

NIMG_Engine::~NIMG_Engine() {
  // Do nothing
}

void NIMG_Engine::load_file(const std::string& path) {
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

void NIMG_Engine::load_nimg(const std::string& path) {
  image = RenderD7::NIMG_Load(path);
}

void NIMG_Engine::draw_pixel(int x, int y, unsigned int color) {
  if (x > image.width || x < 0 || y > image.height || y < 0) return;
  RenderD7::Color::RGBA splitter(color);
  image.pixel_buffer[((y * image.width + x) * 4) + 0] = splitter.m_r;
  image.pixel_buffer[((y * image.width + x) * 4) + 1] = splitter.m_g;
  image.pixel_buffer[((y * image.width + x) * 4) + 2] = splitter.m_b;
  image.pixel_buffer[((y * image.width + x) * 4) + 3] = splitter.m_a;
}

void NIMG_Engine::draw_rect(int x, int y, int w, int h, unsigned int color,
                            int t) {}

void NIMG_Engine::draw_rect_solid(int x, int y, int w, int h,
                                  unsigned int color) {
  for (int ix = x; ix < x + w; ix++) {
    for (int iy = y; iy < y + h; iy++) {
      draw_pixel(ix, iy, color);
    }
  }
}

void NIMG_Engine::flip(bool h, bool v) {
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