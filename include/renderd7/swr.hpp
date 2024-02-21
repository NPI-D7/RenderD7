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