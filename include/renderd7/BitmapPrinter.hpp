#pragma once
#include <string>
#include <vector>

#include <renderd7/bmp.hpp>
#include <renderd7/bmpconverter.hpp>

#include <renderd7/Image.hpp>

#include <renderd7/Time.hpp>
#include <renderd7/Screen.hpp>

namespace RenderD7
{
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
        bool DrawScreenDirectF(int framerate);
        bool DrawScreenDirect();
        void DrawScreenF(int framerate);
        void DrawScreen();
        bool UpdateScreenF(int framerate);
        bool UpdateScreen();
        void Clear(u8 b = 0, u8 g = 0, u8 r = 0, u8 a = 255);
        void ClearBlank();
        RenderD7::Image GetImage();
        /// Test to Find out The Best Settings for BitmapPrinter
        void Benchmark();
        /// Setup the Benchmark
        /// \param framerate The Fps of the ScreenUpdates
        void SetupBenchmark(int framerate);
        bool IsBenchmarkRunning() { return this->benchmark; }

        void DrawText(int x, int y, float t_size, u32 color, std::string text);
        private:
        //funcs
        void DrawChar(u32 posX, u32 posY, u32 color, char character);
        //parameter
        int frame = 0;
        RenderD7::Image renderframe;
        bool isscreen = false;
        C3D_RenderTarget* targetr;
        BMP bitmap = BMP(20, 20, true); //Need to Set e Predefined Bitmap. If not the System will Crash.
        BMP blank = BMP(20, 20, true);  //Need to Set e Predefined Bitmap. If not the System will Crash.

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        //Benchmark Stuff;
        bool benchmark = false;
        bool setupbenchmark;
        float frametime = 0;
        uint64_t lastTime = 0;
        float dtt = 0.f;
        float dtt2 = 0.f;
        float dtt3 = 0.f;
        float timer = 0;
        float mhdtt = 0;
        float mdtt2;
        float mdtt3;

        float fpsClock = 0.f;
	    int frameCounter = 0, fps = 0;

        std::vector<float> hdttt;
        std::vector<float> hdttt2;
        std::vector<float> hdttt3;
        std::vector<int> fpscountc;
        int renderedframes = 0;
        int testfps = 60;
        ////////////////////////////////////////////////////////////////////////////////////////////////
    };
}