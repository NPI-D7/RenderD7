#include <renderd7/Error.hpp>

#include <3ds.h>
#include <renderd7/renderd7.hpp>
#include <renderd7/internal_db.hpp>
#include <renderd7/UI7.hpp>

namespace RenderD7 {
    void Error(const std::string& msg) {
        if(rd7i_graphics_on) {
            C3D_FrameEnd(0);
            while(aptMainLoop()) {
                hidScanInput();
                if(hidKeysDown() & KEY_START) break;
                C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
                RenderD7::ClearTextBufs();
                C2D_TargetClear(Top, 0x00000000);
                C2D_TargetClear(Bottom, 0x00000000);
                RenderD7::OnScreen(Top);
                if(UI7::BeginMenu("RenderD7 - Error Manager", R7Vec2(), UI7MenuFlags_TitleMid)) {
                    UI7::Label(msg, RD7TextFlags_Wrap);
                    UI7::Label("Press Start to Exit!");
                    UI7::EndMenu();
                }
                RenderD7::OnScreen(Bottom);
                C3D_FrameEnd(0);
            }
            exit(0);
        } else {
            gfxInitDefault();
            consoleInit(GFX_TOP, NULL);
            printf("RENDERD7 - ERROR MANAGER\n\n%s\n",  msg.c_str());
            printf("Press Start to Exit\n");
            while(aptMainLoop()) {
                hidScanInput();
                if(hidKeysDown() & KEY_START)
                    break;
                gfxSwapBuffers();
            }
            gfxExit();
            exit(0);
        }
    }
}