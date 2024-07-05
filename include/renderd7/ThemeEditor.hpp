#pragma once

#include <renderd7/renderd7.hpp>

namespace RenderD7 {
class ThemeEditor : public RenderD7::Scene {
 public:
  ThemeEditor();
  ~ThemeEditor();

  void Draw(void) const override;
  void Logic() override;

 private:
  Theme::Ref edit_theme;
  // Placeholder to save active one to
  Theme::Ref temp_theme;

  // temp vars for samples
  mutable bool cm;
  mutable std::string inpt;
  mutable int menu = 0;

  // Keyboard
  mutable RD7KeyboardState kbd_state;
  mutable std::string kbd_text;
  mutable std::vector<std::string> theme_list;
};
}  // namespace RenderD7