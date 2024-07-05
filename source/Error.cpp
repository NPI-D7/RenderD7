#include <3ds.h>

#include <renderd7/Error.hpp>
#include <renderd7/UI7.hpp>
#include <renderd7/internal_db.hpp>
#include <renderd7/renderd7.hpp>

void rd7i_save_report(const std::string& msg) {
  auto ts = RenderD7::GetTimeStr();
  std::ofstream f("sdmc:/RenderD7/Reports/report_" + ts + ".txt");
  f << "RenderD7 Error [" << rd7i_app_name << ", " << ts << "]" << std::endl;
  f << "Error Message: " << std::endl;
  f << msg << std::endl;
  f << "SysInfo: " << std::endl;
  f << "- Citra -> " << (rd7i_is_citra ? "true" : "false") << std::endl;
  f.close();
}

namespace RenderD7 {
void Error(const std::string& msg) {
  rd7i_save_report(msg);
  if (rd7i_graphics_on) {
    while (aptMainLoop()) {
      hidScanInput();
      if (hidKeysDown() & KEY_START) break;
      C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
      RenderD7::ClearTextBufs();
      C2D_TargetClear(rd7_top, 0x00000000);
      C2D_TargetClear(rd7_bottom, 0x00000000);
      RenderD7::R2()->OnScreen(R2Screen_Top);
      if (UI7::BeginMenu("RenderD7 - Error Manager", R7Vec2(),
                         UI7MenuFlags_TitleMid)) {
        UI7::Label(msg, RD7TextFlags_Wrap);
        UI7::Label("Press Start to Exit!");
        UI7::EndMenu();
      }
      RenderD7::R2()->OnScreen(R2Screen_Bottom);
      UI7::Update();
      RenderD7::R2()->Process();
      C3D_FrameEnd(0);
    }
    exit(0);
  } else {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printf("RENDERD7 - ERROR MANAGER\n\n%s\n", msg.c_str());
    printf("Report Saved in\nsdmc:/RenderD7/Reports\n");
    printf("Press Start to Exit\n");
    while (aptMainLoop()) {
      hidScanInput();
      if (hidKeysDown() & KEY_START) break;
      gfxSwapBuffers();
    }
    gfxExit();
    exit(0);
  }
}
}  // namespace RenderD7