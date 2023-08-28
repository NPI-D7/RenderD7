#include <rd7.hpp>

int main() {
  rd7_do_splash = true;
  RenderD7::Ftrace::Beg("app", "app_init");
  RenderD7::Init::Main("rd7tf");
  RenderD7::Init::NdspFirm();
  RenderD7::Ftrace::Beg("app", f2s(RenderD7::LoadSettings));
  RenderD7::LoadSettings();
  RenderD7::Ftrace::End("app", f2s(RenderD7::LoadSettings));
  sound snd("romfs:/thesound.wav", 0);
  snd.play();
  RenderD7::PushMessage(RenderD7::Message(std::string("RenderD7 ") + RENDERD7VSTRING, "Testing the ToastsV2"));
  RenderD7::Ftrace::End("app", "app_init");
  while (RenderD7::MainLoop()) {
    RenderD7::Ftrace::Beg("app", "app_mainloop");
    if (d7_hDown & KEY_START)
      RenderD7::ExitApp();
    if(d7_hDown & KEY_A) {
      RenderD7::PushMessage(RenderD7::Message(std::string("RenderD7 ") + RENDERD7VSTRING, "Testing the ToastsV2\nPressed A"));
    }
    RenderD7::FrameEnd();
    RenderD7::Ftrace::End("app", "app_mainloop");
  }

  return 0;
}