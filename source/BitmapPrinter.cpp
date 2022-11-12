#include <renderd7/BitmapPrinter.hpp>
#include <renderd7/Ovl.hpp>
#include <renderd7/Toast.hpp>
#include <renderd7/stringtool.hpp>

extern bool shouldbe_disabled;
extern std::string csvpc;

RenderD7::BitmapPrinter::BitmapPrinter(int w, int h) {
  BMP newmap(w, h, true);
  bitmap = newmap;
  // renderframe.LoadPFromBuffer(BitmapConverter::ConvertData(bitmap.DATA()));
  blank = newmap;
}

RenderD7::BitmapPrinter::~BitmapPrinter() {
  if (this->renderframe.loadet)
    this->renderframe.Unload();
}

bool RenderD7::BitmapPrinter::DecodeFile(std::string file) {
  unsigned error = bitmap.read(file.c_str());

  if (error) {
    RenderD7::AddOvl(std::make_unique<RenderD7::Toast>(
        "BitmapPrinter", "Error Code: " + std::to_string(error)));
    return false;
  }

  return true;
}

void RenderD7::BitmapPrinter::DrawPixel(int x, int y, u8 b, u8 g, u8 r, u8 a) {
  unsigned error =
      bitmap.set_pixel(x, bitmap.bmp_info_header.height - y, b, g, r, a);
  if (error) {
    RenderD7::AddOvl(std::make_unique<RenderD7::Toast>(
        "BitmapPrinter->Pixel", "Error Code: " + std::to_string(error)));
  }
}
void RenderD7::BitmapPrinter::DrawRect(int x, int y, int w, int h, u8 line_w,
                                       u8 b, u8 g, u8 r, u8 a) {
  unsigned error = bitmap.draw_rectangle(
      x, bitmap.bmp_info_header.height - y - h, w, h, b, g, r, a, line_w);
  if (error) {
    RenderD7::AddOvl(std::make_unique<RenderD7::Toast>(
        "BitmapPrinter->Rect", "Error Code: " + std::to_string(error)));
  }
}

void RenderD7::BitmapPrinter::DrawRectFilled(int x, int y, int w, int h, u8 b,
                                             u8 g, u8 r, u8 a) {
  unsigned error = bitmap.fill_region(x, bitmap.bmp_info_header.height - h - y,
                                      w, h, b, g, r, a);
  if (error) {
    RenderD7::AddOvl(std::make_unique<RenderD7::Toast>(
        "BitmapPrinter->RectF", "Error Code: " + std::to_string(error)));
  }
}

void RenderD7::BitmapPrinter::SaveBmp(std::string name) {
  if (!RenderD7::NameIsEndingWith(name, {"bmp"})) {
    name += ".bmp";
  }
  bitmap.write(name.c_str());
}
void RenderD7::BitmapPrinter::SavePng(std::string name) {
  if (!RenderD7::NameIsEndingWith(name, {"png"})) {
    name += ".png";
  }
  std::vector<unsigned char> ImageBuffer;
  ImageBuffer = BitmapConverter::ConvertData(bitmap.DATA());
  lodepng::save_file(ImageBuffer, name);
}

void RenderD7::BitmapPrinter::CreateScreen(C3D_RenderTarget *target) {
  isscreen = true;
  targetr = target;
  if (target == Top) {
    bitmap = BMP(400, 240, true);
    blank = BMP(400, 240, true);
  }
  if (target == TopRight) {
    bitmap = BMP(400, 240, true);
    blank = BMP(400, 240, true);
  }
  if (target == Bottom) {
    bitmap = BMP(320, 240, true);
    blank = BMP(320, 240, true);
  }
  renderframe.LoadPFromBuffer(BitmapConverter::ConvertData(bitmap.DATA()));
}
bool RenderD7::BitmapPrinter::DrawScreenDirectF(int framerate) {
  bool updtt = false;
  if (isscreen) {
    if (frame == (60 / framerate)) {
      RenderD7::OnScreen(targetr);
      if (renderframe.loadet)
        renderframe.Unload();
      this->Decode(decc);
      frame = 0;
      updtt = true;
    }

    if (renderframe.loadet)
      renderframe.Draw(0, 0);
    frame++;
  }
  return updtt;
}

bool RenderD7::BitmapPrinter::DrawScreenDirect() {
  bool updtt = false;
  if (isscreen) {
    RenderD7::OnScreen(targetr);
    if (renderframe.loadet)
      renderframe.Unload();
    this->Decode(decc);
    updtt = true;
    if (renderframe.loadet)
      renderframe.Draw(0, 0);
  }
  return updtt;
}

RenderD7::Image RenderD7::BitmapPrinter::GetImage() {
  RenderD7::Image img;
  img.LoadFromBitmap(bitmap);
  return img;
}

void RenderD7::BitmapPrinter::UsePreMap(BMP map) { bitmap = map; }
void RenderD7::BitmapPrinter::UsePrePrintMap(BitmapPrinter printmap) {
  bitmap = printmap.GetBitmap();
}

void RenderD7::BitmapPrinter::Clear(u8 b, u8 g, u8 r, u8 a) {
  bitmap.fill_region(0, 0, bitmap.bmp_info_header.width,
                     bitmap.bmp_info_header.height, b, g, r, a);
}
void RenderD7::BitmapPrinter::ClearBlank() { bitmap = blank; }

void RenderD7::BitmapPrinter::DrawScreenF(int framerate) {
  if (isscreen) {
    if (frame == (60 / framerate)) {
      RenderD7::OnScreen(targetr);
      frame = 0;
    }

    if (renderframe.loadet)
      renderframe.Draw(0, 0);
    frame++;
  }
}
void RenderD7::BitmapPrinter::DrawScreen() {
  if (isscreen) {
    RenderD7::OnScreen(targetr);
    if (renderframe.loadet)
      renderframe.Draw(0, 0);
  }
}
bool RenderD7::BitmapPrinter::UpdateScreenF(int framerate) {
  bool updtt = false;
  if (isscreen) {
    if (frame == (60 / framerate)) {
      if (renderframe.loadet)
        renderframe.Unload();
      // renderframe.LoadFromBitmap(bitmap);
      this->Decode(decc);
      frame = 0;
      updtt = true;
    }
    frame++;
  }
  return updtt;
}
bool RenderD7::BitmapPrinter::UpdateScreen() {
  bool updtt = false;
  if (isscreen) {
    if (renderframe.loadet)
      renderframe.Unload();
    this->Decode(decc);
    updtt = true;
  }
  return updtt;
}

#define TICKS_PER_MSEC 268111.856

void RenderD7::BitmapPrinter::Benchmark() {
  if (setupbenchmark) {
    frametime = 0;
    renderedframes = 0;
    timer = 0;
    setupbenchmark = false;
    lastTime = svcGetSystemTick();
  }
  if (benchmark) {
    if (timer >= 60) {
      std::string renderedf = std::to_string(renderedframes);
      std::string avgdtt = std::to_string(mhdtt);
      float alldtt = 0;
      for (size_t i = 1; i < hdttt.size(); i++) {
        alldtt += hdttt[i];
      }
      float alldtt2 = 0;
      for (size_t i = 0; i < hdttt2.size(); i++) {
        alldtt2 += hdttt2[i];
      }
      float alldtt3 = 0;
      for (size_t i = 0; i < hdttt3.size(); i++) {
        alldtt3 += hdttt3[i];
      }
      int allfps = 0;
      for (size_t f = 1; f < fpscountc.size(); f++) {
        allfps += fpscountc[f];
      }

      std::string avgcpu = std::to_string((alldtt / (float)hdttt.size() - 1));
      std::string avgcpu2 =
          std::to_string(((alldtt2 / (float)hdttt2.size()) * 1000));
      std::string avgcpu3 =
          std::to_string(((alldtt3 / (float)hdttt3.size()) * 1000));
      std::string avgfps = std::to_string((allfps / (int)fpscountc.size() - 1));

      std::string resultt =
          "TestMode: " + std::to_string(testfpsd) + "fps" +
          "\nRendered Frames: " + renderedf + "\nMax Cpu Time: " + avgdtt +
          "\nAvg Cpu Time: " + avgcpu + "\nAvg Fps: " + avgfps +
          "\nAvg EncodeTime: " + avgcpu2 + "ms/f\nAvg DecodeTime: " + avgcpu3 +
          "ms\n";
      this->ClearBlank();
      this->DrawRectFilled(0, 0, this->bitmap.bmp_info_header.width,
                           this->bitmap.bmp_info_header.height, 0, 0, 0, 255);
      this->DrawDebugText(0, 0, 0, RenderD7::Color::Hex("#ffffff"), resultt);
      std::string outname =
          csvpc + "/benchmark_" + RenderD7::GetTimeStr() + ".png";
      this->SavePng(outname);
      RenderD7::AddOvl(std::make_unique<RenderD7::Toast>(
          "Benchmark", "Saved to: \n" + outname));
      benchmark = false;
    }
    uint64_t currentTime = svcGetSystemTick();
    dtt = ((float)(currentTime / (float)TICKS_PER_MSEC) -
           (float)(lastTime / (float)TICKS_PER_MSEC)) /
          1000.f;
    lastTime = currentTime;
    lastTime = currentTime;
    frameCounter++;
    fpsClock += dtt;
    if (fpsClock >= 1.f) {
      fps = frameCounter;
      frameCounter = 0;
      fpsClock = 0.f;
    }
    uint64_t lastTime2 = svcGetSystemTick();
    this->ClearBlank();
    this->DrawRectFilled(0, 0, this->bitmap.bmp_info_header.width,
                         this->bitmap.bmp_info_header.width, 255, 255, 255,
                         255);
    this->DrawRect(5, 5, this->bitmap.bmp_info_header.width - 10,
                   this->bitmap.bmp_info_header.height - 10, 5, 0, 0, 0, 0);
    // this->DrawDebugText(20, 20, 0, RenderD7::Color::Hex("#ffffff"), "Fps: " +
    // std::to_string(fps));
    this->DrawDebugText(0, 0, 0.5f, RenderD7::Color::Hex("#ff0000"),
                        "Time: " + std::to_string(timer));
    this->DrawDebugText(0, 10, 0.5f, RenderD7::Color::Hex("#ff0000"),
                        "Fps: " + std::to_string(fps));
    this->DrawDebugText(0, 20, 0.5f, RenderD7::Color::Hex("#ff0000"),
                        "dt: " + std::to_string(dtt));
    this->DrawDebugText(0, 30, 0.5f, RenderD7::Color::Hex("#ff0000"),
                        "MaxEncodeTime: " + std::to_string(mdtt2 * 1000) +
                            "ms/f");
    this->DrawDebugText(0, 40, 0.5f, RenderD7::Color::Hex("#ff0000"),
                        "MaxDecodeTime: " + std::to_string(mdtt3 * 1000) +
                            "ms");
    uint64_t currentTime2 = svcGetSystemTick();
    dtt2 = ((float)(currentTime2 / (float)TICKS_PER_MSEC) -
            (float)(lastTime2 / (float)TICKS_PER_MSEC)) /
           1000.f;
    hdttt2.push_back(dtt2);
    lastTime2 = svcGetSystemTick();
    bool updgg = this->UpdateScreenF(testfps);
    currentTime2 = svcGetSystemTick();
    dtt3 = ((float)(currentTime2 / (float)TICKS_PER_MSEC) -
            (float)(lastTime2 / (float)TICKS_PER_MSEC)) /
           1000.f;
    if (updgg)
      hdttt3.push_back(dtt3);
    if (!shouldbe_disabled)
      this->DrawScreen();
    renderedframes++;
    if (mdtt2 < dtt2) {
      mdtt2 = dtt2;
    }
    if (mdtt3 < dtt3 && updgg) {
      mdtt3 = dtt3;
    }
    timer += 1 * dtt;
    float hdtt = C3D_GetProcessingTime();
    hdttt.push_back(hdtt);
    fpscountc.push_back(fps);
    if (mhdtt < hdtt) {
      mhdtt = C3D_GetProcessingTime();
    }
    /*if (!shouldbe_disabled)
    {
            RenderD7::OnScreen(Bottom);
            RenderD7::DrawText(0, 0, 0.5f, RenderD7::Color::Hex("#ff0000"),
    "Time: " + std::to_string(timer)); RenderD7::DrawText(0, 20, 0.5f,
    RenderD7::Color::Hex("#ff0000"), "Fps: " + std::to_string(fps));
            RenderD7::DrawText(0, 40, 0.5f, RenderD7::Color::Hex("#ff0000"),
    "dt: " + std::to_string(dtt)); RenderD7::DrawText(0, 60, 0.5f,
    RenderD7::Color::Hex("#ff0000"), "MaxRenderTime: " +
    std::to_string(mdtt2*1000) + "ms/f"); RenderD7::DrawText(0, 80, 0.5f,
    RenderD7::Color::Hex("#ff0000"), "MaxConvertTime: " +
    std::to_string(mdtt3*1000) + "ms");

    }*/
  }
}

void RenderD7::BitmapPrinter::SetupBenchmark(int framerate) {
  benchmark = true;
  setupbenchmark = true;
  this->testfps = framerate;
  this->testfpsd = framerate;
}

#include <renderd7/debugfont.h>

void RenderD7::BitmapPrinter::DrawDebugChar(u32 posX, u32 posY, int t_size,
                                            u32 color, char character) {
  bool isscale = (t_size > 1) ? true : false;
  for (u32 y = 0; y < 8; y++) {
    char charPos = debugfont[character * 8 + y];

    for (u32 x = 0; x < 8; x++)
      if (((charPos >> (7 - x)) & 1) == 1) {
        if (!isscale)
          DrawPixel((int)posX + x + 1, (int)posY + y + 1, UNPACK_BGRA(color));
        if (isscale)
          DrawRectFilled(((int)posX) + (x * t_size) + 1,
                         ((int)posY) + (y * t_size) + 1, t_size, t_size,
                         UNPACK_BGRA(color));
      }
  }
}

void RenderD7::BitmapPrinter::DrawChar(int posX, int posY, float t_size,
                                       u32 color, char character,
                                       RenderD7::NFontApi font) {
  for (int y = 0; y < font.GetGlyphHeight(character); y++) {
    for (int x = 0; x < font.GetGlyphWidth(character); x++) {
      DrawPixel(posX + x + 1, posY + y + 1, 255, 255, 255,
                font.GetGlyphBitmap(
                    character)[((y * font.GetGlyphWidth(character) + x) * 1)]);
      if (((font.GetGlyphBitmap(
                character)[font.GetGlyphHeight(character) + y] >>
            (font.GetGlyphWidth(character - 1) - x)) &
           1) == 1) {
      }
    }
  }
  // for(int y = 0; y < font.GetGlyphHeight(character) *
  // font.GetGlyphWidth(character); y++)
  //{
  //    DrawPixel(posX + x + 1, posY + y + 1, UNPACK_BGRA(color));
  //}
}

#define SPACING_Y 10
#define SPACING_X 8

void RenderD7::BitmapPrinter::DrawDebugText(int x, int y, int t_size, u32 color,
                                            std::string text) {
  if (t_size < 1) {
    t_size = 1;
  }

  for (u32 i = 0, line_i = 0; i < strlen(text.c_str()); i++)
    switch (text[i]) {
    case '\n':
      y += (SPACING_Y * t_size);
      line_i = 0;
      break;

    case '\t':
      line_i += 2;
      break;

    default:
      // Make sure we never get out of the screen
      if (line_i >= (((u32)this->bitmap.bmp_info_header.width) - (u32)x) /
                        (SPACING_X * t_size)) {
        y += (SPACING_Y * t_size);
        line_i = 1; // Little offset so we know the same text continues
        if (text[i] == ' ')
          break; // Spaces at the start look weird
      }

      this->DrawDebugChar((u32)x + line_i * (SPACING_X * t_size), (u32)y,
                          t_size, color, text[i]);

      line_i++;
      break;
    }
}

void RenderD7::BitmapPrinter::DrawText(int x, int y, float t_size, u32 color,
                                       std::string text,
                                       RenderD7::NFontApi font) {
  if (t_size < 1) {
    t_size = 1;
  }

  for (u32 i = 0, line_i = 0; i < strlen(text.c_str()); i++)
    switch (text[i]) {
    case '\n':
      y += (font.GetLineHeight());
      line_i = 0;
      break;

    case '\t':
      line_i += 2;
      break;

    default:
      // Make sure we never get out of the screen
      if (line_i >= (((u32)this->bitmap.bmp_info_header.width) - (u32)x) /
                        (u32)(font.GetGlyphWidth(text[i]))) {
        y += (SPACING_Y * t_size);
        line_i = 1; // Little offset so we know the same text continues
        if (text[i] == ' ')
          break; // Spaces at the start look weird
      }

      this->DrawChar(x + line_i * (font.GetGlyphWidth(text[i])), y, t_size,
                     color, text[i], font);

      line_i++;
      break;
    }
}

bool RenderD7::BitmapPrinter::Decode(Decoder deccc) {
  bool res = false;

  switch (deccc) {
  case Decoder::BITMAP2PNG2C3D:
    renderframe.LoadPFromBuffer(
        BitmapConverter::ConvertData(this->bitmap.DATA()));
    res = true;
    break;
  case Decoder::BITMAP2C3D:
    renderframe.LoadFromBitmap(this->bitmap);
    res = true;
    break;

  default:
    res = false;
    break;
  }
  return res;
}
