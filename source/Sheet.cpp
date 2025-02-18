#include <renderd7/Sheet.hpp>

RenderD7::Sheet::Sheet() {
  //
}
RenderD7::Sheet::~Sheet() {
  //
}

Result RenderD7::Sheet::Load(const std::string& path) {
  this->spritesheet = C2D_SpriteSheetLoad(path.c_str());
  return 0;
}

void RenderD7::Sheet::Free() { C2D_SpriteSheetFree(this->spritesheet); }
