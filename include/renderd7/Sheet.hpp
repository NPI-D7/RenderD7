#pragma once
#include <3ds.h>  // Result
#include <citro2d.h>
#include <citro3d.h>

#include <string>

namespace RenderD7 {
/// @brief SpriteSheet Class
class Sheet {
 public:
  /// @brief Constructor
  Sheet() = default;
  /// @brief Deconstructor
  ~Sheet() {
    if (spritesheet) Free();
  }
  /// @brief Load A Spritesheet File
  /// @param path Path to the t3x
  /// @return Result Code
  Result Load(const std::string& path);
  /// @brief Unload the Sheet
  void Free();
  /// \param spritesheet The Sheet
  C2D_SpriteSheet spritesheet;
};
}  // namespace RenderD7