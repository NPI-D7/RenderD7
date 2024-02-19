#pragma once

#include <renderd7/nimg.hpp>

namespace RenderD7 {
class NIMG_Engine {
 public:
  NIMG_Engine(int w, int h);
  NIMG_Engine();
  ~NIMG_Engine();
  nimg& GetImage() { return image; }
  void load_file(const std::string& path);
  void load_nimg(const std::string& path);

  // Rendering
  void draw_pixel(int x, int y, unsigned int color);
  void draw_rect(int x, int y, int w, int h, unsigned int color, int t = 1);
  void draw_rect_solid(int x, int y, int w, int h, unsigned int color);
  void flip(bool h, bool v);

 private:
  nimg image;
};
}  // namespace RenderD7