#include "scene.hpp"

void ColorThemeSample() {
  RD7::ColorNew(RD7Color_Text, RD7::Color::Hex("#000000", 255));
  RD7::ColorNew(RD7Color_TextDisabled, RD7::Color::Hex("#212121", 200));
  RD7::ColorNew(RD7Color_Text2, RD7::Color::Hex("#ffffff", 255));
  RD7::ColorNew(RD7Color_Background, RD7::Color::Hex("#eeeeee", 170));
  RD7::ColorNew(RD7Color_Header, RD7::Color::Hex("#222222", 190));
  RD7::ColorNew(RD7Color_Selector, RD7::Color::Hex("#4444dd", 200));
  RD7::ColorNew(RD7Color_SelectorFade, RD7::Color::Hex("#7777dd", 200));
  RD7::ColorNew(RD7Color_List0, RD7::Color::Hex("#555555", 130));
  RD7::ColorNew(RD7Color_List1, RD7::Color::Hex("#777777", 130));
  RD7::ColorNew(RD7Color_MessageBackground, RD7::Color::Hex("#222222", 180));
  RD7::ColorNew(RD7Color_Button, RD7::Color::Hex("#4444dd", 150));
  RD7::ColorNew(RD7Color_ButtonHovered, RD7::Color::Hex("#6666dd", 150));
  RD7::ColorNew(RD7Color_ButtonDisabled, RD7::Color::Hex("#2222dd", 150));
  RD7::ColorNew(RD7Color_ButtonActive, RD7::Color::Hex("#7777dd", 150));
  RD7::ColorNew(RD7Color_Checkmark, RD7::Color::Hex("#4444dd", 130));
  RD7::ColorNew(RD7Color_FrameBg, RD7::Color::Hex("#555555", 160));
  RD7::ColorNew(RD7Color_FrameBgHovered, RD7::Color::Hex("#777777", 160));
  RD7::ColorNew(RD7Color_Progressbar, RD7::Color::Hex("#4444dd", 200));
}

extern void IdbServer();

int main() {
  rd7_enable_memtrack = true;
  rd7_do_splash = true;
  RD7::Ftrace::Beg("app", "app_init");
  RD7::Init::Main("rd7tf");
  RD7::FadeIn();
  // IdbServer();
  ColorThemeSample();
  RD7::Init::NdspFirm();
  RD7::Scene::Load(std::make_unique<Sample>());
  RD7::Ftrace::End("app", "app_init");
  while (RD7::MainLoop()) {
    RD7::OnScreen(Top);
    RD7::Ftrace::Beg("app", "app_mainloop");
    if (d7_hDown & KEY_START) {
      RD7::FadeOut();
      RD7::ExitApp();
    }
    RD7::OnScreen(Top);
    RD7::FrameEnd();
    RD7::Ftrace::End("app", "app_mainloop");
  }

  return 0;
}