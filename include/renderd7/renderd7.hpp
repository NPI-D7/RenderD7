#pragma once
#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <memory>
#include <stack>
#include <string>
#include <functional>
#include <map>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <random>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <locale>
#include <time.h>

#include <codecvt>

#include <renderd7/external/lodepng.h>
#include <renderd7/lang.hpp>
#include <renderd7/parameter.hpp>
#include <renderd7/thread.hpp>
#include <renderd7/ini.hpp>
#include <renderd7/stringtool.hpp>
#include <renderd7/bmp.hpp>
#include <renderd7/bmpconverter.hpp>
#include <renderd7/Toast.hpp>
#include <renderd7/Ovl.hpp>
#include <renderd7/BitmapPrinter.hpp>
#include <renderd7/Image.hpp>
#include <renderd7/Sprite.hpp>
#include <renderd7/SpriteAnimation.hpp>
#include <renderd7/Sheet.hpp>
#include <renderd7/Color.hpp>
#include <renderd7/Time.hpp>
#include <renderd7/Screen.hpp>

extern "C"
{
    #include <renderd7/external/fs.h>
}

#define RENDERD7VSTRING "0.8.0"
#define CHANGELOG "0.8.0: Implement BitmapPrinter\n0.7.3: Implement Over Render Overlay Framework\n0.7.2: Implement MT to csv file saving. Add RGB2HEX. \n0.7.1: Add the New Overlay Handler. Its Just in code and does nothing yet. \n0.7.0: Made Big Progress In the MT Ovl but it still crashes On a Scnd C3D_FrameEnd(). Implement 800px but doesn't work that good. \n0.6.2: Fix Crash when exiting trouth Home Menu.  \n0.6.10: rewrite Threadsystem, Improve framerate\n0.6.02: Fix Code in lang.hpp\nadd Draw Text Left Function.\nadd changelog\n0.6.01: add Threading system."
#define DEFAULT_CENTER 0.5f

/*extern C3D_RenderTarget* Top;
extern C3D_RenderTarget* TopRight;
extern C3D_RenderTarget* Bottom;*/

extern u32 d7_hDown;
extern u32 d7_hHeld;
extern u32 d7_hUp;
extern touchPosition d7_touch;

extern std::string dspststus;

/// RenderD7
namespace RenderD7
{
    float GetDeltaTime();
    enum kbd{
         SWKBD,
         BKBD
    };
    enum kbd_type
    {
         NUMPAD,
         STANDARD
    };
    struct TObject
    {
        int x; //Position X
        int y; //Position Y
        int w; //Button Width
        int h; //Button Height
        std::string text = ""; //Text
        float correctx = 0; //Correct X Position
        float correcty = 0; //Correct Y Position
        float txtsize = 0.7f;  //Set Text Size
    };

    class Scene {
    public:
        static std::stack<std::unique_ptr<Scene>> scenes;
    	virtual ~Scene() {}
    	virtual void Logic(u32 hDown, u32 hHeld, u32 hUp, touchPosition touch) = 0;
    	virtual void Draw() const = 0;
        //virtual void Ovl() const = 0;
        static void Load(std::unique_ptr<Scene> scene, bool fade = false);
        static void Back();
        static void doDraw();
        static void doLogic(u32 hDown, u32 hHeld, u32 hUp, touchPosition touch);
        //static void HandleOvl();
    };

    class RSettings : public RenderD7::Scene
    {
    private:
        enum RState {
            RSETTINGS,
            RINFO
        };
        RenderD7::RSettings::RState m_state = RenderD7::RSettings::RState::RSETTINGS;

        std::string rd7srstate = "false";
        std::string csvstate = "false";
        std::string mtovlstate = "false";
        std::string fpsstate = "60";
        std::string mtscreenstate = "Top";
        std::string mttxtcolstate = "#ffffff";
        std::string mtcola = "255";
        std::string mttxtcola = "255";

        
        std::vector<RenderD7::TObject> buttons = 
        {
            {20, 35, 120, 35, "RD7SR", -11, 10},
            {20, 85, 120, 35, "MT_CSV", -15, 9},
            {20, 135, 120, 35, "MT_OVL", -19, 10},
            {20, 185, 120, 35, "FPS", 6, 10},
            {180, 35, 120, 35, "MTSCREEN", -29, 10},
            {180, 85, 120, 35, "DSPERR", -13, 10},
            {180, 135, 120, 35, "INFO", 2, 10},
            {180, 185, 120, 35, "", -13, 10}
        };
    public:
        RSettings();
        void Draw(void) const override;
        ~RSettings();
        void Logic(u32 hDown, u32 hHeld, u32 hUp, touchPosition touch) override;
    };

    void LoadSettings();

    class DSP_NF : public RenderD7::Ovl
    {
        public:
        DSP_NF();
        void Draw(void) const override;
        void Logic() override;
        private:
        int msgposy = 240;
        int delay = 0;
    };

    
    int GetRandomInt(int b, int e);
    void DrawMetrikOvl();
    bool DrawImageFromSheet(RenderD7::Sheet* sheet, size_t index, float x, float y, float scaleX = 1.0, float scaleY = 1.0);
    namespace Error
    {
        void DisplayError(std::string toptext, std::string errortext, int timesec = 3);
        void DisplayFatalError(std::string toptext, std::string errortext);
    }
    namespace Init
    {
        Result Main(std::string app_name = "RD7Game");
        Result Reload();
        void Graphics();
        void NdspFirm(bool useit = false);
    }
    namespace Exit
    {
        void Main();
        void NdspFirm();
        void Graphics();
    }
    namespace Msg
    {
        void Display(std::string titletxt, std::string subtext, C3D_RenderTarget *target);
        void DisplayWithProgress(std::string titletext, std::string subtext, float current, float total, u32 prgbarcolor);
    }
    
    namespace Convert
    {
        inline float StringtoFloat(std::string inp){return std::atof(inp.c_str());}
        inline int StringtoInt(std::string inp){return std::atoi(inp.c_str());}
        inline bool FloatToBool(float inp){if(inp == 1)return true; else return false;}
    }

    struct DirContent
    {
        std::string name;
        std::string path;
        bool isDir;
    };

    namespace FS
    {
        bool FileExist(const std::string& path);
    }
    
    bool IsNdspInit();
    void SetupLog(void);
    std::string GetFramerate();
    bool MainLoop();
    void ExitApp();
    
    void ClearTextBufs(void);
   
    std::string Kbd(int lenght, SwkbdType tp);
    bool DrawRect(float x, float y, float w, float h, u32 color);
    bool DrawNFRect(float p1x, float p1y, float w, float h, u32 color, float scale = 1);
    bool DrawPx(float x, float y, u32 color);
    void DrawTextCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth = 0, int maxHeight = 0, C2D_Font fnt = nullptr);
	void DrawText(float x, float y, float size, u32 color, std::string Text, int maxWidth = 0, int maxHeight = 0, C2D_Font fnt = nullptr);
    void DrawTextLeft(float x, float y, float size, u32 color, std::string Text, int maxWidth = 0, int maxHeight = 0, C2D_Font fnt = nullptr);
	float GetTextWidth(float size, std::string Text, C2D_Font fnt = nullptr);
	void GetTextSize(float size, float *width, float *height, std::string Text, C2D_Font fnt = nullptr);
	float GetTextHeight(float size, std::string Text, C2D_Font fnt = nullptr);
    Result loadFont(C2D_Font &fnt, const char * Path = "");
	Result unloadFont(C2D_Font &fnt);
    bool DrawCircle(float x, float y, float radius, u32 color);
    bool DrawImage(C2D_Image img, float x, float y, float scaleX = 1.0f, float scaleY = 1.0f);
    void FrameEnd();
    void ToggleRD7SR();
    bool IsRD7SR();
    

    struct TLBtn
    {
        int x; //Position X
        int y; //Position Y
        int w; //Button Width
        int h; //Button Height
    };

    struct ScrollList1 
    {
        std::string Text = "";
    };

    struct ScrollList2 
    {
        float x;
        float y;
        float w;
        float h;
        std::string Text = "";
    };
    /*enum ListType
    {
        ONE,
        TWO
    };*/
    void DrawList1(RenderD7::ScrollList1 &l, float txtsize, C3D_RenderTarget *t);
    void DrawTObjects(std::vector<RenderD7::TObject> tobjects, u32 color, u32 txtcolor,  int selection = -1, u32 selbgcolor = RenderD7::Color::Hex("#2D98AF"), u32 selcolor = RenderD7::Color::Hex("#000000"));
    void DrawSTObject(std::vector<RenderD7::TObject> tobject, int tobjectindex, u32 color, u32 txtcolor);
    bool touchTObj(touchPosition touch, RenderD7::TObject button);
    bool touchTLBtn(touchPosition touch, RenderD7::TLBtn button);
    void DrawTLBtns(std::vector<RenderD7::TLBtn> btns, u32 color,  int selection = -1, u32 selbgcolor = RenderD7::Color::Hex("#2D98AF"), u32 selcolor = RenderD7::Color::Hex("#000000"));
    
    struct Checkbox
    {
        float x, y, s;
        bool is_chexked = false;
        u32 outcol, incol, chcol;
    };
    void DrawCheckbox(Checkbox box);
    
    class Console
    {
         public:
           Console();
           Console(int x, int y, int w, int h, u8 a = 255);
           Console(int x, int y, int w, int h, RenderD7::Color::rgba col);
           Console(int x, int y, int w, int h, std::string name, RenderD7::Color::rgba col = {255, 255, 255, 255}, RenderD7::Color::rgba barcol = {0, 0, 0, 255}, RenderD7::Color::rgba outlinecol = {222, 222, 222, 255});
           void On(C3D_RenderTarget *t_cscreen);
           bool Update();
           ~Console();
         private:
           std::vector<std::string> m_lines;
           int x, y, w, h;
           std::string m_name = "";
           C3D_RenderTarget *cscreen;
           bool m_nconsole = false;
           bool m_mconsole = false;
           RenderD7::Color::rgba color = {255, 255, 255, 255};
           RenderD7::Color::rgba outlinecol = {222, 222, 222, 255};
           RenderD7::Color::rgba barcolor = {0, 0, 0, 255};
    };

    void GetDirContentsExt(std::vector<RenderD7::DirContent> &dircontent, const std::vector<std::string> &extensions);
    void GetDirContents(std::vector<RenderD7::DirContent> &dircontent);

} /// RenderD7
