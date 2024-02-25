#include "scene.hpp"

extern void IdbServer();

int main() {
  rd7_enable_memtrack = true;
  rd7_do_splash = true;
  RD7::Ftrace::Beg("app", "app_init");
  RD7::Init::Main("rd7tf");
  RD7::FadeIn();
  // IdbServer();
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