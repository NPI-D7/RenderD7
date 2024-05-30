#pragma once

#include <renderd7/renderd7.hpp>

namespace RenderD7 {
class ThemeEditor : public RenderD7::Scene {
 public:
  ThemeEditor() = default;
  ~ThemeEditor() = default;

  void Draw() const override;
  void Logic() override;

 private:
};
}  // namespace RenderD7