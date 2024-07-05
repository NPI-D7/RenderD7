#include <renderd7/Sheet.hpp>
#include <renderd7/internal_db.hpp>

Result RenderD7::Sheet::Load(const std::string& path) {
  if (this->spritesheet) Free();
  this->spritesheet = C2D_SpriteSheetLoad(path.c_str());
  if (!this->spritesheet) {
    _rd7i_logger()->Write("Failed to Load Spritesheet from: " + path, 0);
  }
  return 0;
}

void RenderD7::Sheet::Free() {
  if (!this->spritesheet) return;
  C2D_SpriteSheetFree(this->spritesheet);
  this->spritesheet = nullptr;
}

C2D_Image RenderD7::Sheet::GetImage(int idx) {
  if (!this->spritesheet) return {nullptr, nullptr};
  return C2D_SpriteSheetGetImage(this->spritesheet, idx);
}