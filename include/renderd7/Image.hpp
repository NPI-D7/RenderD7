#pragma once

#include <3ds.h>
#include <citro2d.h>

#include <renderd7/R7Vec.hpp>
#include <renderd7/nimg.hpp>
#include <renderd7/smart_ctor.hpp>
#include <string>

namespace RenderD7 {
class Image {
 public:
  Image() = default;
  Image(C2D_Image img) { this->img = img; }
  Image(const std::string& path) { this->Load(path); }
  ~Image() = default;
  RD7_SMART_CTOR(Image)
  void Load(const std::string& path);
  void From_NIMG(const nimg& image);
  void Delete();

  C2D_Image Get();
  C2D_Image& GetRef();
  void Set(const C2D_Image& i);
  R7Vec2 GetSize();
  bool Loadet();

 private:
  bool ext = false;
  C2D_Image img;
};
}  // namespace RenderD7