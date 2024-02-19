#include "scene.hpp"

void ColorThemeSample() {
  RenderD7::ColorNew(RD7Color_Text, RenderD7::Color::Hex("#000000", 255));
  RenderD7::ColorNew(RD7Color_TextDisabled,
                     RenderD7::Color::Hex("#212121", 200));
  RenderD7::ColorNew(RD7Color_Text2, RenderD7::Color::Hex("#ffffff", 255));
  RenderD7::ColorNew(RD7Color_Background, RenderD7::Color::Hex("#eeeeee", 170));
  RenderD7::ColorNew(RD7Color_Header, RenderD7::Color::Hex("#222222", 190));
  RenderD7::ColorNew(RD7Color_Selector, RenderD7::Color::Hex("#4444dd", 200));
  RenderD7::ColorNew(RD7Color_SelectorFade,
                     RenderD7::Color::Hex("#7777dd", 200));
  RenderD7::ColorNew(RD7Color_List0, RenderD7::Color::Hex("#555555", 130));
  RenderD7::ColorNew(RD7Color_List1, RenderD7::Color::Hex("#777777", 130));
  RenderD7::ColorNew(RD7Color_MessageBackground,
                     RenderD7::Color::Hex("#222222", 180));
  RenderD7::ColorNew(RD7Color_Button, RenderD7::Color::Hex("#4444dd", 150));
  RenderD7::ColorNew(RD7Color_ButtonHovered,
                     RenderD7::Color::Hex("#6666dd", 150));
  RenderD7::ColorNew(RD7Color_ButtonDisabled,
                     RenderD7::Color::Hex("#2222dd", 150));
  RenderD7::ColorNew(RD7Color_ButtonActive,
                     RenderD7::Color::Hex("#7777dd", 150));
  RenderD7::ColorNew(RD7Color_Checkmark, RenderD7::Color::Hex("#4444dd", 130));
  RenderD7::ColorNew(RD7Color_FrameBg, RenderD7::Color::Hex("#555555", 160));
  RenderD7::ColorNew(RD7Color_FrameBgHovered,
                     RenderD7::Color::Hex("#777777", 160));
  RenderD7::ColorNew(RD7Color_Progressbar,
                     RenderD7::Color::Hex("#4444dd", 200));
}

extern void IdbServer();

int main() {
  rd7_enable_memtrack = true;
  rd7_do_splash = true;
  RenderD7::Ftrace::Beg("app", "app_init");
  RenderD7::Init::Main("rd7tf");
  RenderD7::FadeIn();
  // IdbServer();
  ColorThemeSample();
  RenderD7::Init::NdspFirm();
  RenderD7::Scene::Load(std::make_unique<Sample>());
  RenderD7::Ftrace::End("app", "app_init");
  while (RenderD7::MainLoop()) {
    RenderD7::OnScreen(Top);
    RenderD7::Ftrace::Beg("app", "app_mainloop");
    if (d7_hDown & KEY_START) {
      RenderD7::FadeOut();
      RenderD7::ExitApp();
    }
    RenderD7::OnScreen(Top);
    RenderD7::FrameEnd();
    RenderD7::Ftrace::End("app", "app_mainloop");
  }

  return 0;
}