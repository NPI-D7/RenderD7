#pragma once

#include <renderd7/Ovl.hpp>

namespace RenderD7 {
class Ovl_Ftrace : public RenderD7::Ovl {
  /// @brief Constructor
  Ovl_Ftrace();
  /// @brief Override for Draw
  void Draw(void) const override;
  /// @brief Override for Logic
  void Logic() override;

private:
};
} // namespace RenderD7