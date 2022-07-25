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
#include <renderd7/Clock.hpp>
#include <renderd7/bmp.hpp>
#include <renderd7/bmpconverter.hpp>

extern "C"
{
    #include <renderd7/external/fs.h>
}

#define RENDERD7VSTRING "0.8.0"
#define CHANGELOG "0.8.0: Implement BitmapPrinter\n0.7.3: Implement Over Render Overlay Framework\n0.7.2: Implement MT to csv file saving. Add RGB2HEX. \n0.7.1: Add the New Overlay Handler. Its Just in code and does nothing yet. \n0.7.0: Made Big Progress In the MT Ovl but it still crashes On a Scnd C3D_FrameEnd(). Implement 800px but doesn't work that good. \n0.6.2: Fix Crash when exiting trouth Home Menu.  \n0.6.10: rewrite Threadsystem, Improve framerate\n0.6.02: Fix Code in lang.hpp\nadd Draw Text Left Function.\nadd changelog\n0.6.01: add Threading system."
#define DEFAULT_CENTER 0.5f

extern C3D_RenderTarget* Top;
extern C3D_RenderTarget* TopRight;
extern C3D_RenderTarget* Bottom;

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
    /// Set current RenderScreen
    /// \param target The RenderTarget Top, Bottom
    void OnScreen(C3D_RenderTarget *target);
    /** The Spritesheet Class */
    class Sheet
    {
        public:
        /// Construct sheet
        Sheet();
        // Deconstruct sheet
        ~Sheet();
        /// Load a Sritesheet
        /// \param path Path to the Spritesheet (.t3x)
        Result Load(const char *path);
        /// Unload the Spritesheet
        void Free();
        /// The Spritesheet
        C2D_SpriteSheet spritesheet;
    };
    /// Image Class
    class Image
    {
        public:
        ~Image();

        void Unload();
        /// Load Image from Png
        /// \param path path to png file
        void LoadPng(const std::string path);
        /// Load the Image from buffer
        /// \param buffer the frame buffer
        void LoadPFromBuffer(const std::vector<u8> &buffer);
        void LoadFromBitmap(BMP bitmap);
        /// Draw the Image directly
        /// \param x The x position
        /// \param y the y position
        /// \param scaleX x scale from 0.0 to 1.0
        /// \param scaleY y scale from 0.0 to 1.0
        bool Draw(float x, float y, float scaleX = 1.0f, float scaleY = 1.0f);
        /// \brief Get The Image
        /// \return C2D_Image
        C2D_Image Get(){return this->img;}

        void FromSheet(RenderD7::Sheet sheet, size_t index);
        /// \param img this is the C2D_Image
        C2D_Image img;
        /// \param loadet whether the image is loadet or not
        bool loadet = false;
    };
    /// Sprite Class
    class Sprite
    {
        public:
        /// \brief Construct Sprite
        Sprite();
        /// \brief Deconstruct Sprite
        ~Sprite();
        /// \brief Load a Sprite From SpriteSheet
        /// \param sheet the Sheet to load from.(RenderD7::Sheet)
        /// \param index the number of the Sprite in the Sheet
        void FromSheet(RenderD7::Sheet *sheet, size_t index);
        /// \brief Load a Sprite From SpriteSheet
        /// \param img the Image to load from.(RenderD7::Image)
        void FromImage(RenderD7::Image *img);
        bool Draw();
        void SetCenter(float x, float y);
        void SetPos(float x, float y);
        void SetScale(float x, float y);
        void SetRotation(float rotation);
        void Rotate(float speed);
        float getWidth();
        float getHeigh();
        float getPosX();
        float getPosY();
        private:
        C2D_ImageTint tint;
        C2D_Sprite sprite;
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

    class Ovl {
        public:
        virtual ~Ovl(){}
        virtual void Draw() const = 0;
        virtual void Logic() = 0;
        inline bool IsKilled() {return this->iskilled; }
        inline void Kill() { iskilled = true; }
        private:
        bool iskilled = false;
    };

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

    class Toast : public RenderD7::Ovl
    {
        public:
        Toast(std::string head, std::string msg);
        void Draw(void) const override;
        void Logic() override;
        private:
        std::string head, msg;
        int msgposy = 240;
        int delay = 0;
    };

    void AddOvl(std::unique_ptr<RenderD7::Ovl> scene);
    namespace Color
    {
        struct rgba
        {
            u8 r, g, b, a;
        };
        class RGBA{
            public:
            RGBA(u8 r, u8 g, u8 b, u8 a) : m_r(r),m_g(g),m_b(b),m_a(a){}
            u32 toRGBA() const {return (m_r << 24) | (m_g << 16) | (m_b << 8) | m_a;}
            
            u8 m_r, m_g ,m_b, m_a;
        };
        std::string RGB2Hex(int r, int g, int b);
        u32 Hex(const std::string color, u8 a = 255);
    }
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

    class SpriteSheetAnimation : public RenderD7::Sprite
    {
        public:
        SpriteSheetAnimation();
        ~SpriteSheetAnimation();
        void Setup(RenderD7::Sheet *sheet, size_t imagecount, size_t startimage, float frame_begin, float frame_finish);
        void Play(float timespeed);
        private:
        size_t images;
        size_t imgs = 0;
        float D_totaltime;
        RenderD7::Sheet *sheet;
        float time;
    };
    

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
    std::string FormatString(std::string fmt_str, ...);
    std::string GetTimeStr(void);
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

    bool NameIsEndingWith(const std::string &name, const std::vector<std::string> &extensions);
    void GetDirContentsExt(std::vector<RenderD7::DirContent> &dircontent, const std::vector<std::string> &extensions);
    void GetDirContents(std::vector<RenderD7::DirContent> &dircontent);

    class BitmapPrinter
    {
        public:
        BitmapPrinter(int w, int h);
        ~BitmapPrinter();
        bool DecodeFile(std::string file);
        
        bool DecodeMem(std::vector<unsigned char> buffer);
        void DrawPixel(int x, int y, u8 b, u8 g, u8 r, u8 a);
        void DrawRect(int x, int y, int w, int h, u8 line_w, u8 b, u8 g, u8 r, u8 a);
        void DrawRectFilled(int x, int y, int w, int h, u8 b, u8 g, u8 r, u8 a);
        void UsePreMap(BMP map);
        void UsePrePrintMap(BitmapPrinter printmap);
        BMP GetBitmap(){ return bitmap; }
        void SaveBmp(std::string name);
        void SavePng(std::string name);

        void CreateScreen(C3D_RenderTarget *target);
        void DrawScreenDirectF(int framerate);
        void DrawScreenDirect();
        void DrawScreenF(int framerate);
        void DrawScreen();
        void UpdateScreenF(int framerate);
        void UpdateScreen();
        void Clear(u8 b = 0, u8 g = 0, u8 r = 0, u8 a = 255);
        void ClearBlank();
        RenderD7::Image GetImage();
        /// Test to Find out The Best Settings for BitmapPrinter
        void Benchmark();
        /// Setup the Benchmark
        /// \param framerate The Fps of the ScreenUpdates
        void SetupBenchmark(int framerate);
        private:
        int frame = 0;
        RenderD7::Image renderframe;
        bool isscreen = false;
        C3D_RenderTarget* targetr;
        BMP bitmap = NULL;
        BMP blank = NULL;
        //Benchmark Stuff;
        bool benchmark = false;
        bool setupbenchmark;
        float frametime = 0;
        uint64_t lastTime = 0;
        float dtt = 0.f;
        float timer = 0;
        float mhdtt = 0;

        float fpsClock = 0.f;
	    int frameCounter = 0, fps = 0;

        std::vector<float> hdttt;
        std::vector<int> fpscountc;
        int renderedframes = 0;
        int testfps = 60;
    };
} /// RenderD7