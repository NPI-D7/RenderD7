#pragma once
#include <3ds.h>  // Result
#include <citro2d.h>
#include <citro3d.h>

#include <renderd7/Image.hpp>
#include <renderd7/smart_ctor.hpp>
#include <string>

namespace RenderD7 {
/// @brief SpriteSheet Class
class Sheet {
 public:
  /// @brief Constructor
  Sheet() = default;
  Sheet(const std::string& path) { this->Load(path); }
  /// @brief Deconstructor
  ~Sheet() {
    if (spritesheet) Free();
  }
  RD7_SMART_CTOR(Sheet);
  /// @brief Load A Spritesheet File
  /// @param path Path to the t3x
  /// @return Result Code
  Result Load(const std::string& path);
  /// @brief Unload the Sheet
  void Free();
  C2D_Image GetImage(int idx);
  C2D_SpriteSheet Get() { return this->spritesheet; }

 private:
  /// \param spritesheet The Sheet
  C2D_SpriteSheet spritesheet;
};
}  // namespace RenderD7