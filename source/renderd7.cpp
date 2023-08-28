#include <regex>
#include <renderd7/log.hpp>
#include <renderd7/renderd7.hpp>
#include <renderd7/renderd7_logo.hpp>

#define D7_NOTHING 0x00000000
#define CFGVER "6"

bool isndspinit = false;
bool running = true;
std::stack<std::unique_ptr<RenderD7::Scene>> RenderD7::Scene::scenes;
std::unique_ptr<RenderD7::Scene> tmpFadeS;
/// @brief  Supports Multiple Overlays
std::vector<std::unique_ptr<RenderD7::Ovl>> overlays;
/// @brief Displays Overlays step by step from first 2 last
std::vector<std::unique_ptr<RenderD7::Ovl>> toast_overlays;
std::string dspststus = "Not Initialisized!";

int cobj___;
int maxobj__;
bool rd7_do_splash = false;

// INI::INIFile cfgfile;
std::unique_ptr<INI::INIFile> cfgfile = nullptr;
INI::INIStructure cfgstruct;
std::string cfgpath;

u8 consoleModel = 0;
u8 sysRegion = CFG_REGION_USA;
bool is_citra = false;
//---------------------------------------
bool rd7settings = false;

std::string D_app_name;

u32 d7_hDown;
u32 d7_hHeld;
u32 d7_hUp;
touchPosition d7_touch;

C2D_TextBuf TextBuf;
C2D_Font Font;

// Fps Calc
static float current_fps = 0.0f;
static unsigned int frames = 0;
static u64 last_time = 0;
float d11framerate = 0;
//-----------------

// Metrik-------------------------------------
u32 mt_color;
u32 mt_txtcolor;

std::string mt_fps;
std::string mt_cpu;
std::string mt_gpu;
std::string mt_cmd;
std::string mt_lfr;

int mt_screen;
// int mt_width = mt_screen ? 320 : 400;
float mt_txtSize;
bool metrikd = false;
// double mt_fpsgraph[320];
std::vector<int> mt_fpsgraph(320);

//-------------------------------------------
bool currentScreen = false;

C3D_RenderTarget *Top;
C3D_RenderTarget *TopRight;
C3D_RenderTarget *Bottom;

#define DSEVENBLACK 0xff000000
#define DSEVENWHITE 0xffffffff

u64 delta_time;
u64 last_tm;
float dtm;

// Lets define them to prevent from red lines in vscode lol
#ifndef V_TIME
#define V_TIME "0"
#endif
#ifndef V_STRING
#define V_STRING "0"
#endif

// Screen Fade
bool fadeout = false, fadein = false, fadeout2 = false, fadein2 = false;
int fadealpha = 0;
int fadecolor = 0;
bool waitFade = false;
bool FadeExit = false;
bool SceneFadeWait = false;

std::vector<std::string> string_to_lines(std::string input_str) {
  std::vector<std::string> lines;
  std::stringstream ss(input_str);
  std::string line;
  while (std::getline(ss, line)) {
    lines.push_back(line);
  }
  return lines;
}

void Npifade() {
  if (fadein) {
    if (fadealpha < 255) {
      fadealpha += 3;
    } else {
      fadein = false;
    }
  } else if (fadeout) {
    if (fadealpha > 0) {
      fadealpha -= 3;
    } else {
      fadeout = false;
    }
  } else {
    if (waitFade)
      waitFade = false;
    if (FadeExit)
      running = false;
    if (SceneFadeWait) {
      RenderD7::Scene::scenes.push(std::move(tmpFadeS));
      SceneFadeWait = false;
    }
    // No fade
  }
  RenderD7::OnScreen(Top);
  RenderD7::Draw::Rect(0, 0, 400, 240, ((fadealpha << 24) | 0x00000000));
  RenderD7::OnScreen(Bottom);
  RenderD7::Draw::Rect(0, 0, 320, 240, ((fadealpha << 24) | 0x00000000));
}

void PushSplash() {
  C2D_SpriteSheet sheet;
  sheet = C2D_SpriteSheetLoadFromMem((void *)renderd7_logo, renderd7_logo_size);
  // Display for 2 Sec
  for (int x = 0; x < 120; x++) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(Top, DSEVENBLACK);
    C2D_TargetClear(Bottom, DSEVENBLACK);
    RenderD7::ClearTextBufs();
    RenderD7::OnScreen(Top);
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(sheet, 0), 400 / 2 - 300 / 2,
                    240 / 2 - 100 / 2, 0.5);
    C3D_FrameEnd(0);
  }
  C2D_SpriteSheetFree(sheet);
}

std::string _FMT_(const std::string &fmt_str, ...) {
  va_list ap;
  char *fp = NULL;
  va_start(ap, fmt_str);
  vasprintf(&fp, fmt_str.c_str(), ap);
  va_end(ap);
  std::unique_ptr<char, decltype(free) *> formatted(fp, free);
  return std::string(formatted.get());
}

std::string Date(void) {
  time_t unixTime;
  struct tm timeStruct;
  time(&unixTime);
  localtime_r(&unixTime, &timeStruct);
  return _FMT_("%04i-%02i-%02i_%02i-%02i-%02i", timeStruct.tm_year + 1900,
               timeStruct.tm_mon + 1, timeStruct.tm_mday, timeStruct.tm_hour,
               timeStruct.tm_min, timeStruct.tm_sec);
}

void screenoff() {
  gspLcdInit();
  GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTH);
  gspLcdExit();
}

void screenon() {
  gspLcdInit();
  GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);
  gspLcdExit();
}

float RenderD7::GetDeltaTime() { return (float)dtm; }

bool RenderD7::DrawImageFromSheet(RenderD7::Sheet *sheet, size_t index, float x,
                                  float y, float scaleX, float scaleY) {
  if (sheet->spritesheet != nullptr) {
    if (C2D_SpriteSheetCount(sheet->spritesheet) >= index) {
      return C2D_DrawImageAt(C2D_SpriteSheetGetImage(sheet->spritesheet, index),
                             x, y, 0.5f, nullptr, scaleX, scaleY);
    }
  }
  return false;
}
void RenderD7::Init::NdspFirm() {
  if (access("sdmc:/3ds/dspfirm.cdc", F_OK) != -1) {
    rd7_security->SafeInit(ndspInit, ndspExit);
    isndspinit = true;
    dspststus = "Initialisized success!";
  } else {
    dspststus = "Not found: dspfirm.cdc";
    RenderD7::AddToast(std::make_unique<RenderD7::DSP_NF>());
  }
}

void RenderD7::Msg::Display(std::string titletxt, std::string subtext,
                            C3D_RenderTarget *target) {
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C2D_TargetClear(Top, DSEVENBLACK);
  C2D_TargetClear(Bottom, DSEVENBLACK);
  RenderD7::ClearTextBufs();
  RenderD7::OnScreen(Top);
  RenderD7::Draw::Rect(0, 0, 400, 240, RenderD7::Color::Hex("#111111"));
  RenderD7::OnScreen(Bottom);
  RenderD7::Draw::Rect(0, 0, 320, 240, RenderD7::Color::Hex("#111111"));
  RenderD7::OnScreen(target);
  RenderD7::Draw::Rect(0, 0, 400, 26, RenderD7::Color::Hex("#333333", 200));
  RenderD7::Draw::Text(5, 2, 0.7f, DSEVENWHITE, titletxt);
  RenderD7::Draw::Text(5, 30, 0.6f, DSEVENWHITE, subtext);
  C3D_FrameEnd(0);
}
void RenderD7::Msg::DisplayWithProgress(std::string titletext,
                                        std::string subtext, float current,
                                        float total, u32 prgbarcolor) {
  RenderD7::ClearTextBufs();
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C2D_TargetClear(Top, DSEVENBLACK);
  C2D_TargetClear(Bottom, DSEVENBLACK);
  char str[256];
  snprintf(str, sizeof(str), "(%.2f%%)",
           ((float)current / (float)total) * 100.0f);
  RenderD7::OnScreen(Top);
  RenderD7::Draw::Rect(0, 0, 400, 240, RenderD7::Color::Hex("#111111"));
  RenderD7::Draw::Rect(0, 0, 400, 26, RenderD7::Color::Hex("#333333", 200));
  RenderD7::Draw::Text(5, 2, 0.7f, DSEVENWHITE, titletext);
  RenderD7::Draw::Text(5, 30, 0.6f, DSEVENWHITE, subtext);
  RenderD7::Draw::Rect(30, 120, 342, 30, RenderD7::Color::Hex("#333333"));
  RenderD7::Draw::Rect(31, 121, (int)(((float)current / (float)total) * 338.0f),
                       28, prgbarcolor);
  RenderD7::Draw::TextCentered(5, 124, 0.7f, RenderD7::Color::Hex("#111111"),
                               str, 390);
  RenderD7::OnScreen(Bottom);
  RenderD7::Draw::Rect(0, 0, 320, 240, RenderD7::Color::Hex("#111111"));
  C3D_FrameEnd(0);
}

void RenderD7::Error::DisplayError(std::string toptext, std::string errortext,
                                   int timesec) {
  RenderD7::ClearTextBufs();
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C2D_TargetClear(Top, DSEVENBLACK);
  C2D_TargetClear(Bottom, DSEVENBLACK);
  RenderD7::OnScreen(Top);
  RenderD7::Draw::Text(0, 0, 0.7f, DSEVENWHITE, toptext);
  RenderD7::Draw::Text(0, 30, 0.6f, DSEVENWHITE, errortext);
  C3D_FrameEnd(0);
  for (int i = 0; i < 60 * timesec; i++) {
    RenderD7::Draw::Rect(0, 236,
                         (int)(((float)i / (float)60 * timesec) * 400.0f), 4,
                         RenderD7::Color::Hex("#00ff00"));
    gspWaitForVBlank();
  }
}

void RenderD7::Error::DisplayFatalError(std::string toptext,
                                        std::string errortext) {
  bool error___ = true;
  RenderD7::ClearTextBufs();
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C2D_TargetClear(Top, RenderD7::Color::Hex("#000000"));
  C2D_TargetClear(Bottom, RenderD7::Color::Hex("#000000"));
  RenderD7::OnScreen(Top);
  RenderD7::Draw::TextCentered(0, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                               toptext, 400);
  RenderD7::Draw::TextCentered(0, 100, 0.6f, RenderD7::Color::Hex("#ffffff"),
                               errortext, 400);
  RenderD7::Draw::TextCentered(0, 200, 0.6f, RenderD7::Color::Hex("#ffffff"),
                               "Press Start to Exit!", 400);
  C3D_FrameEnd(0);
  while (error___) {
    if (d7_hDown & KEY_START) {
      error___ = false;
      RenderD7::ExitApp();
    }
  }
}

void RenderD7::Scene::doDraw() {
  if (!RenderD7::Scene::scenes.empty())
    RenderD7::Scene::scenes.top()->Draw();
}

void RenderD7::Scene::doLogic(u32 hDown, u32 hHeld, u32 hUp,
                              touchPosition touch) {
  if (!RenderD7::Scene::scenes.empty())
    RenderD7::Scene::scenes.top()->Logic(hDown, hHeld, hUp, touch);
}

void RenderD7::Scene::Load(std::unique_ptr<Scene> scene, bool fade) {
  if (fade) {
    tmpFadeS = std::move(scene);
    SceneFadeWait = true;
  } else
    Scene::scenes.push(std::move(scene));
}

void RenderD7::Scene::Back() {
  if (RenderD7::Scene::scenes.size() > 0)
    RenderD7::Scene::scenes.pop();
}

void frameloop() {
  frames++;
  delta_time = osGetTime() - last_time;
  if (delta_time >= 1000) {
    current_fps = frames / (delta_time / 1000.0f) + 1;
    frames = 0;
    last_time = osGetTime();
  }
  d11framerate = current_fps;
  for (int i = 0; i < 320; i++)
    mt_fpsgraph[i] = current_fps;
}
float getframerate() { return d11framerate; }

std::string RenderD7::GetFramerate() {
  return (std::to_string((int)d11framerate).substr(0, 2));
}

bool RenderD7::MainLoop() {
  RenderD7::Ftrace::Beg("rd7-core", f2s(RenderD7::MainLoop));
  if (!aptMainLoop())
    return false;

  // Deltatime
  uint64_t currentTime = svcGetSystemTick();
  dtm = ((float)(currentTime / (float)TICKS_PER_MSEC) -
         (float)(last_tm / (float)TICKS_PER_MSEC)) /
        1000.f;
  last_tm = currentTime;

  hidScanInput();
  d7_hDown = hidKeysDown();
  d7_hUp = hidKeysUp();
  d7_hHeld = hidKeysHeld();
  hidTouchRead(&d7_touch);

  RenderD7::ClearTextBufs();
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

  C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
  C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
  frameloop();
  RenderD7::Ftrace::Beg("rd7sm", f2s(RenderD7::Scene::doDraw));
  RenderD7::Scene::doDraw();
  RenderD7::Ftrace::End("rd7sm", f2s(RenderD7::Scene::doDraw));
  RenderD7::Ftrace::Beg("rd7sm", f2s(RenderD7::Scene::doLogic));
  RenderD7::Scene::doLogic(d7_hDown, d7_hHeld, d7_hUp, d7_touch);
  RenderD7::Ftrace::End("rd7sm", f2s(RenderD7::Scene::doLogic));

  // Disably Overlays For one Frame
  RenderD7::Ftrace::End("rd7-core", f2s(RenderD7::MainLoop));
  return running;
}

void RenderD7::ClearTextBufs(void) { C2D_TextBufClear(TextBuf); }

void RenderD7::Init::Graphics() {
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(size_t(maxobj__));
  C2D_Prepare();
  Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  TopRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  TextBuf = C2D_TextBufNew(4096);
  Font = C2D_FontLoadSystem(CFG_REGION_USA);
}

Result RenderD7::Init::Main(std::string app_name) {
  /// The only func that can be executed before Security
  RenderD7::Ftrace::Beg("rd7-core", f2s(RenderD7::Init::Main));
  RenderD7::Init::Security();
  rd7_security->SafeInit(gfxInitDefault, gfxExit);
  // consoleInit(GFX_TOP, NULL);
  // Dont safinit this cause it has security
  Result res = cfguInit();
  if (R_SUCCEEDED(res)) {
    CFGU_SecureInfoGetRegion(&sysRegion);
    CFGU_GetSystemModel(&consoleModel);
    cfguExit();
  }
  printf("cfgu\n");

  rd7_security->SafeInit(aptInit, aptExit);
  rd7_security->SafeInit(romfsInit, romfsExit);

  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  rd7_security->SafeExit(C3D_Fini);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  rd7_security->SafeExit(C2D_Fini);
  C2D_Prepare();
  Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  TopRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  TextBuf = C2D_TextBufNew(4096);
  Font = C2D_FontLoadSystem(CFG_REGION_USA);
  printf("Graphical Interface\n");
  last_tm = svcGetSystemTick();
  RenderD7::Ftrace::Beg("rd7-core", "do_splash");
  if (rd7_do_splash)
    PushSplash();
  RenderD7::Ftrace::End("rd7-core", "do_splash");

  printf("stuff\n");
  if (cobj___) {
    maxobj__ = cobj___;
  }
  if (!cobj___) {
    maxobj__ = C2D_DEFAULT_MAX_OBJECTS;
  }
  D_app_name = app_name;
  cfgpath = "sdmc:/RenderD7/Apps/";
  cfgpath += D_app_name;
  std::filesystem::create_directories(cfgpath.c_str());
  bool renew = false;
  printf("folderset\n");
  if (FS::FileExist(cfgpath + "/config.ini")) {
    cfgfile = std::make_unique<INI::INIFile>(cfgpath + "/config.ini");
    cfgfile->read(cfgstruct);
    std::string version = cfgstruct["info"]["version"];
    if (version != CFGVER)
      renew = true;
  }
  printf("vercheck\n");
  if (!FS::FileExist(cfgpath + "/config.ini") || renew) {
    cfgfile = std::make_unique<INI::INIFile>(cfgpath + "/config.ini");
    cfgfile->read(cfgstruct);
    cfgstruct.clear();
    cfgstruct["info"]["version"] = CFGVER;
    cfgstruct["info"]["renderd7ver"] = RENDERD7VSTRING;
    cfgstruct["settings"]["doscreentimeout"] = "0";
    cfgstruct["settings"]["forcetimeoutLB"] = "1";
    cfgstruct["settings"]["renderer"] = "c3d_c2d";
    cfgstruct["metrik-settings"]["enableoverlay"] = "0";
    cfgstruct["metrik-settings"]["Screen"] = "0";
    cfgstruct["metrik-settings"]["txtColor"] = "#ffffffff";
    cfgstruct["metrik-settings"]["Color"] = "#aa000000";
    cfgstruct["metrik-settings"]["txtSize"] = "0.7f";
    cfgfile->write(cfgstruct);
  }
  cfgfile = std::make_unique<INI::INIFile>(cfgpath + "/config.ini");
  cfgfile->read(cfgstruct);
  ////C3D_FrameRate(RenderD7::Convert::StringtoFloat(Fps));
  metrikd = RenderD7::Convert::FloatToBool(RenderD7::Convert::StringtoFloat(
      cfgstruct["metrik-settings"]["enableoverlay"]));
  mt_txtSize =
      RenderD7::Convert::StringtoFloat(cfgstruct["metrik-settings"]["txtSize"]);
  mt_screen =
      RenderD7::Convert::StringtoInt(cfgstruct["metrik-settings"]["Screen"]);
  printf("read\n");
  // Check if citra
  s64 citracheck = 0;
  svcGetSystemInfo(&citracheck, 0x20000, 0);
  is_citra = citracheck ? true : false;
  printf("citra\n");
  // Speedup
  osSetSpeedupEnable(true);
  printf("boost\n");
  RenderD7::Ftrace::End("rd7-core", f2s(RenderD7::Init::Main));
  // RenderD7::Msg::Display("RenderD7", "RenderD7 init success!\nWaiting for
  // MainLoop!", Top);
  return 0;
}

Result RenderD7::Init::Minimal(std::string app_name) {
  D_app_name = app_name;
  RenderD7::Init::Security();
  rd7_security->SafeInit(gfxInitDefault, gfxExit);
  rd7_security->SafeInit(romfsInit, romfsExit);

  osSetSpeedupEnable(true);
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  rd7_security->SafeExit(C3D_Fini);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  rd7_security->SafeExit(C2D_Fini);
  C2D_Prepare();
  Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  TopRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  TextBuf = C2D_TextBufNew(4096);
  Font = C2D_FontLoadSystem(CFG_REGION_USA);
  if (rd7_do_splash)
    PushSplash();

  // Check if citra
  s64 citracheck = 0;
  svcGetSystemInfo(&citracheck, 0x20000, 0);
  is_citra = citracheck ? true : false;

  if (cobj___) {
    maxobj__ = cobj___;
  }
  if (!cobj___) {
    maxobj__ = C2D_DEFAULT_MAX_OBJECTS;
  }
  cfgpath = "sdmc:/RenderD7/Apps/";
  cfgpath += D_app_name;
  std::filesystem::create_directories(cfgpath.c_str());
  bool renew = false;
  printf("folderset\n");
  if (FS::FileExist(cfgpath + "/config.ini")) {
    cfgfile = std::make_unique<INI::INIFile>(cfgpath + "/config.ini");
    cfgfile->read(cfgstruct);
    std::string version = cfgstruct["info"]["version"];
    if (version != CFGVER)
      renew = true;
  }
  printf("vercheck\n");
  if (!FS::FileExist(cfgpath + "/config.ini") || renew) {
    cfgfile = std::make_unique<INI::INIFile>(cfgpath + "/config.ini");
    cfgfile->read(cfgstruct);
    cfgstruct.clear();
    cfgstruct["info"]["version"] = CFGVER;
    cfgstruct["info"]["renderd7ver"] = RENDERD7VSTRING;
    cfgstruct["settings"]["doscreentimeout"] = "0";
    cfgstruct["settings"]["forcetimeoutLB"] = "1";
    cfgstruct["settings"]["renderer"] = "c3d_c2d";
    cfgstruct["metrik-settings"]["enableoverlay"] = "0";
    cfgstruct["metrik-settings"]["Screen"] = "0";
    cfgstruct["metrik-settings"]["txtColor"] = "#ffffffff";
    cfgstruct["metrik-settings"]["Color"] = "#aa000000";
    cfgstruct["metrik-settings"]["txtSize"] = "0.7f";
    cfgfile->write(cfgstruct);
  }
  if (renew)
    printf("renew\n");
  cfgfile = std::make_unique<INI::INIFile>(cfgpath + "/config.ini");
  cfgfile->read(cfgstruct);
  // C3D_FrameRate(RenderD7::Convert::StringtoFloat(Fps));
  metrikd = RenderD7::Convert::FloatToBool(RenderD7::Convert::StringtoFloat(
      cfgstruct["metrik-settings"]["enableoverlay"]));
  mt_txtSize =
      RenderD7::Convert::StringtoFloat(cfgstruct["metrik-settings"]["txtSize"]);
  mt_screen =
      RenderD7::Convert::StringtoInt(cfgstruct["metrik-settings"]["Screen"]);
  printf("boost\n");
  return 0;
}

Result RenderD7::Init::Reload() {
  C2D_TextBufDelete(TextBuf);
  C2D_Fini();
  C3D_Fini();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(size_t(maxobj__));
  C2D_Prepare();
  Top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  TopRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  Bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  TextBuf = C2D_TextBufNew(4096);
  Font = C2D_FontLoadSystem(CFG_REGION_USA);

  return 0;
}

void RenderD7::DrawTObjects(std::vector<RenderD7::TObject> tobjects, u32 color,
                            u32 txtcolor, int selection, u32 selbgcolor,
                            u32 selcolor) {
  for (int i = 0; i < (int)tobjects.size(); i++) {
    if (selection == i) {
      RenderD7::Draw::Rect(tobjects[i].x - 2, tobjects[i].y - 2,
                           tobjects[i].w + 4, tobjects[i].h + 4, selbgcolor);
      RenderD7::Draw::Rect(tobjects[i].x, tobjects[i].y, tobjects[i].w,
                           tobjects[i].h, color);
      RenderD7::Draw::Rect(tobjects[i].x, tobjects[i].y, tobjects[i].w,
                           tobjects[i].h, selcolor);
      RenderD7::Draw::Text(tobjects[i].x + (tobjects[i].w / 2) -
                               RenderD7::Draw::GetTextWidth(tobjects[i].txtsize,
                                                            tobjects[i].text) /
                                   2,
                           tobjects[i].y + (tobjects[i].h / 2) -
                               RenderD7::Draw::GetTextHeight(
                                   tobjects[i].txtsize, tobjects[i].text) /
                                   2,
                           tobjects[i].txtsize, txtcolor, tobjects[i].text);
    } else {
      RenderD7::Draw::Rect(tobjects[i].x, tobjects[i].y - 1, tobjects[i].w,
                           tobjects[i].h, color);
      RenderD7::Draw::Text(tobjects[i].x + (tobjects[i].w / 2) -
                               RenderD7::Draw::GetTextWidth(tobjects[i].txtsize,
                                                            tobjects[i].text) /
                                   2,
                           tobjects[i].y + (tobjects[i].h / 2) -
                               RenderD7::Draw::GetTextHeight(
                                   tobjects[i].txtsize, tobjects[i].text) /
                                   2,
                           tobjects[i].txtsize, txtcolor, tobjects[i].text);
    }
  }
}

void RenderD7::DrawTLBtns(std::vector<RenderD7::TLBtn> btns, u32 color,
                          int selection, u32 selbgcolor, u32 selcolor) {
  for (int i = 0; i < (int)btns.size(); i++) {
    if (selection == i) {
      RenderD7::Draw::Rect(btns[i].x - 2, btns[i].y - 2, btns[i].w + 4,
                           btns[i].h + 4, selbgcolor);
      RenderD7::Draw::Rect(btns[i].x, btns[i].y, btns[i].w, btns[i].h, color);
      RenderD7::Draw::Rect(btns[i].x, btns[i].y, btns[i].w, btns[i].h,
                           selcolor);
    } else {
      RenderD7::Draw::Rect(btns[i].x, btns[i].y - 1, btns[i].w, btns[i].h,
                           color);
    }
  }
}

void RenderD7::ExitApp() {
  if (waitFade) {
    FadeExit = true;
  } else
    running = false;
}

bool RenderD7::touchTObj(touchPosition touch, RenderD7::TObject button) {
  if (touch.px >= button.x && touch.px <= (button.x + button.w) &&
      touch.py >= button.y && touch.py <= (button.y + button.h))
    return true;
  else
    return false;
}

bool RenderD7::touchTLBtn(touchPosition touch, RenderD7::TLBtn button) {
  if (touch.px >= button.x && touch.px <= (button.x + button.w) &&
      touch.py >= button.y && touch.py <= (button.y + button.h))
    return true;
  else
    return false;
}

int RenderD7::GetRandomInt(int b, int e) {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(b, e);
  int r = distribution(generator);
  return r;
}

void RenderD7::DrawSTObject(std::vector<RenderD7::TObject> tobject,
                            int tobjectindex, u32 color, u32 txtcolor) {
  RenderD7::Draw::Rect(tobject[tobjectindex].x, tobject[tobjectindex].y,
                       tobject[tobjectindex].w, tobject[tobjectindex].h, color);
  RenderD7::Draw::Text(
      tobject[tobjectindex].x + (tobject[tobjectindex].w / 2) -
          RenderD7::Draw::GetTextWidth(tobject[tobjectindex].txtsize,
                                       tobject[tobjectindex].text) /
              2,
      tobject[tobjectindex].y + (tobject[tobjectindex].h / 2) -
          RenderD7::Draw::GetTextHeight(tobject[tobjectindex].txtsize,
                                        tobject[tobjectindex].text) /
              2,
      tobject[tobjectindex].txtsize, txtcolor, tobject[tobjectindex].text);
}

bool RenderD7::FS::FileExist(const std::string &path) {
  FILE *test = fopen(path.c_str(), "r");
  if (test != NULL) {
    fclose(test);

    return true;
  }

  return false;
}

bool RenderD7::IsNdspInit() {
  if (isndspinit) {
    return true;
  } else {
    return false;
  }
}

std::string priv_fmt_bytes(int bytes) {
  char out[32];

  if (bytes == 1)
    snprintf(out, sizeof(out), "%d Byte", bytes);

  else if (bytes < 1024)
    snprintf(out, sizeof(out), "%d Bytes", bytes);

  else if (bytes < 1024 * 1024)
    snprintf(out, sizeof(out), "%.1f KB", (float)bytes / 1024);

  else if (bytes < 1024 * 1024 * 1024)
    snprintf(out, sizeof(out), "%.1f MB", (float)bytes / 1024 / 1024);

  else
    snprintf(out, sizeof(out), "%.1f GB", (float)bytes / 1024 / 1024 / 1024);

  return out;
}

void RenderD7::DrawMetrikOvl() {
  switch (mt_screen) {
  case 0:
    RenderD7::OnScreen(Top);
    break;
  case 1:
    RenderD7::OnScreen(Bottom);
    break;
  default:
    RenderD7::OnScreen(Bottom);
    break;
  }
  std::string __C = RENDERD7VSTRING;
  std::string info = "RenderD7 " + __C + " Debug Overlay";
  float infoy = 240 - RenderD7::Draw::GetTextHeight(mt_txtSize, info);
  mt_fps = "FPS: " + RenderD7::GetFramerate();
  mt_cpu = "CPU: " +
           std::to_string(C3D_GetProcessingTime() * (d11framerate / 10))
               .substr(0, 4) +
           "%/" + std::to_string(C3D_GetProcessingTime()).substr(0, 4) + "ms";
  mt_gpu =
      "GPU: " +
      std::to_string(C3D_GetDrawingTime() * (d11framerate / 10)).substr(0, 4) +
      "%/" + std::to_string(C3D_GetDrawingTime()).substr(0, 4) + "ms";
  mt_cmd =
      "CMD: " + std::to_string(C3D_GetCmdBufUsage() * 100.0f).substr(0, 4) +
      "%/" + std::to_string(C3D_GetCmdBufUsage()).substr(0, 4) + "ms";
  mt_lfr = "Linear: " + priv_fmt_bytes(linearSpaceFree());
  RenderD7::Draw::Rect(0, 0, RenderD7::Draw::GetTextWidth(mt_txtSize, mt_fps),
                       RenderD7::Draw::GetTextHeight(mt_txtSize, mt_fps),
                       mt_color);
  RenderD7::Draw::Rect(0, 50, RenderD7::Draw::GetTextWidth(mt_txtSize, mt_cpu),
                       RenderD7::Draw::GetTextHeight(mt_txtSize, mt_cpu),
                       mt_color);
  RenderD7::Draw::Rect(0, 70, RenderD7::Draw::GetTextWidth(mt_txtSize, mt_gpu),
                       RenderD7::Draw::GetTextHeight(mt_txtSize, mt_gpu),
                       mt_color);
  RenderD7::Draw::Rect(0, 90, RenderD7::Draw::GetTextWidth(mt_txtSize, mt_cmd),
                       RenderD7::Draw::GetTextHeight(mt_txtSize, mt_cmd),
                       mt_color);
  RenderD7::Draw::Rect(0, 110, RenderD7::Draw::GetTextWidth(mt_txtSize, mt_lfr),
                       RenderD7::Draw::GetTextHeight(mt_txtSize, mt_lfr),
                       mt_color);
  RenderD7::Draw::Rect(0, infoy, RenderD7::Draw::GetTextWidth(mt_txtSize, info),
                       RenderD7::Draw::GetTextHeight(mt_txtSize, info),
                       mt_color);
  RenderD7::Draw::Text(0, 0, mt_txtSize, mt_txtcolor, mt_fps);
  RenderD7::Draw::Text(0, 50, mt_txtSize, mt_txtcolor, mt_cpu);
  RenderD7::Draw::Text(0, 70, mt_txtSize, mt_txtcolor, mt_gpu);
  RenderD7::Draw::Text(0, 90, mt_txtSize, mt_txtcolor, mt_cmd);
  RenderD7::Draw::Text(0, 110, mt_txtSize, mt_txtcolor, mt_lfr);
  RenderD7::Draw::Text(0, infoy, mt_txtSize, mt_txtcolor, info);
}

RenderD7::DSP_NF::DSP_NF() {}

void RenderD7::DSP_NF::Draw(void) const {
  RenderD7::OnScreen(Top);
  RenderD7::Draw::Rect(0, msgposy, 400, 70, RenderD7::Color::Hex("#111111"));
  RenderD7::Draw::Rect(0, msgposy, 400, 25, RenderD7::Color::Hex("#222222"));
  RenderD7::Draw::Text(2, msgposy + 3, 0.7f, RenderD7::Color::Hex("#ffffff"),
                       "RenderD7: Warning!");
  RenderD7::Draw::Text(2, msgposy + 30, 0.6f, RenderD7::Color::Hex("#ffffff"),
                       "You can't use Sound effects because the "
                       "file\n<<sdmc:/3ds/dspfirm.cdc>> was not found!");
}

void RenderD7::DSP_NF::Logic() {
  this->delay++ /*=(int)RenderD7::GetDeltaTime()*/;
  if (msgposy > 170 && delay < 5 * 60)
    msgposy-- /*=(int)RenderD7::GetDeltaTime()*/;

  if (delay >= 5 * 60) {
    msgposy++ /*=(int)RenderD7::GetDeltaTime*/;
    if (msgposy > 400)
      this->Kill();
  }
}

void OvlHandler() {
  for (size_t i = 0; i < toast_overlays.size(); i++) {
    if (toast_overlays[i]->IsKilled())
      toast_overlays.erase(toast_overlays.begin() + i);
  }
  if ((int)toast_overlays.size() > 0) {
    toast_overlays[0]->Draw();
    toast_overlays[0]->Logic();
  }
  for (size_t i = 0; i < overlays.size(); i++) {
    overlays[i]->Draw();
    overlays[i]->Logic();
    if (overlays[i]->IsKilled())
      overlays.erase(overlays.begin() + i);
  }
}

void RenderD7::FrameEnd() {
  C3D_FrameBegin(2);
  if (metrikd)
    RenderD7::DrawMetrikOvl();

  RenderD7::Ftrace::Beg("rd7oh", f2s(OvlHandler));
  OvlHandler();
  RenderD7::Ftrace::End("rd7oh", f2s(OvlHandler));
  Npifade();

  C3D_FrameEnd(0);
}

RenderD7::RSettings::RSettings() {
  RenderD7::FadeIn();
  cfgfile = std::make_unique<INI::INIFile>(cfgpath + "/config.ini");
  cfgfile->read(cfgstruct);
  rd7settings = true;
  lines = string_to_lines(CHANGELOG);
  calculate_screens(lines, screen_index, screens);
}

RenderD7::RSettings::~RSettings() {}

std::vector<std::string> StrHelper(std::string input) {
  std::string ss(input);
  std::istringstream in(ss);
  std::vector<std::string> test1;

  std::copy(std::istream_iterator<std::string>(in),
            std::istream_iterator<std::string>(), std::back_inserter(test1));
  return test1;
}

void RenderD7::RSettings::Draw(void) const {
  if (m_state == RSETTINGS) {
    RenderD7::OnScreen(Top);
    RenderD7::Draw::Rect(0, 0, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Rect(0, 21, 400, 220, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Text(0, 0, 0.7f, DSEVENWHITE, "RenderD7->Settings");
    RenderD7::Draw::TextRight(400, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                              RENDERD7VSTRING);
    RenderD7::Draw::Text(0, 30, 0.7f, DSEVENBLACK,
                         "Metrik Overlay: " + mtovlstate);
    RenderD7::Draw::Text(0, 50, 0.7f, DSEVENBLACK,
                         "Metrik Screen: " + mtscreenstate);
    RenderD7::Draw::Text(
        0, 70, 0.7f, DSEVENBLACK,
        "Current: " + std::to_string(RenderD7::Memory::GetCurrent()) + "b");
    RenderD7::OnScreen(Bottom);
    std::string verc = "Config Version: ";
    verc += CFGVER;
    RenderD7::Draw::Rect(0, 0, 320, 240, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Text(0, 0, 0.7f, RenderD7::Color::Hex("#111111"), verc);
    RenderD7::DrawTObjects(buttons, RenderD7::Color::Hex("#111111"),
                           RenderD7::Color::Hex("#eeeeee"));

  } else if (m_state == RSERVICES) {
    RenderD7::OnScreen(Top);
    RenderD7::Draw::Rect(0, 0, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Rect(0, 21, 400, 220, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Text(0, 0, 0.7f, DSEVENWHITE, "RenderD7->Services");
    RenderD7::Draw::TextRight(400, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                              RENDERD7VSTRING);
    RenderD7::OnScreen(Bottom);
    RenderD7::Draw::Rect(0, 0, 320, 240, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Text(0, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                         "Press B to Get back!");

  } else if (m_state == RCLOG) {
    RenderD7::OnScreen(Top);
    int start_index = screen_index * 9;
    int end_index = std::min((int)lines.size(), start_index + 9);

    RenderD7::Draw::Rect(0, 21, 400, 220, RenderD7::Color::Hex("#eeeeee"));
    for (int i = start_index; i < end_index; i++) {
      RenderD7::Draw::Text(5, 23 + 20 * (i - start_index), 0.7f, DSEVENBLACK,
                           lines[i]);
    }
    RenderD7::Draw::Rect(0, 219, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Text(0, 220, 0.7f, RenderD7::Color::Hex("#ffffff"),
                         "Site: " + std::to_string(screen_index + 1) + "/" +
                             std::to_string(screens + 1));

    RenderD7::Draw::Rect(0, 0, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Text(0, 0, 0.7f, DSEVENWHITE, "RenderD7->Changelog");
    RenderD7::Draw::TextRight(400, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                              RENDERD7VSTRING);
    RenderD7::OnScreen(Bottom);
    RenderD7::Draw::Rect(0, 0, 320, 240, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Rect(0, 0, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Text(0, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                         "Press B to Get back!");

  } else if (m_state == RINFO) {
    std::string rd7ver = RENDERD7VSTRING;
    std::string rd7cfgver = CFGVER;
    std::string citras = is_citra ? "true" : "false";
    std::string buildtime = V_TIME;
    std::string commit = V_STRING;
    RenderD7::OnScreen(Top);
    RenderD7::Draw::Rect(0, 0, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Rect(0, 21, 400, 220, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Text(0, 0, 0.7f, DSEVENWHITE, "RenderD7->Info");
    RenderD7::Draw::TextRight(400, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                              RENDERD7VSTRING);
    RenderD7::Draw::Text(0, 30, 0.7f, DSEVENBLACK, "App: " + D_app_name);
    RenderD7::Draw::Text(0, 50, 0.7f, DSEVENBLACK, "RenderD7: " + rd7ver);
    RenderD7::Draw::Text(0, 70, 0.7f, DSEVENBLACK,
                         "Config-Version: " + rd7cfgver);
    RenderD7::Draw::Text(0, 90, 0.7f, DSEVENBLACK, "Citra: " + citras);
    RenderD7::Draw::Text(0, 110, 0.7f, DSEVENBLACK,
                         "RenderD7-Build-Time: \n" + buildtime);
    RenderD7::Draw::Text(0, 150, 0.7f, DSEVENBLACK,
                         "RenderD7-Commit: " + commit);
    RenderD7::Draw::Text(0, 170, 0.7f, DSEVENBLACK,
                         "RenderD7-Overlays: " +
                             std::to_string(overlays.size()));
    /*RenderD7::Draw::Text(0, 130, 0.7f, DSEVENBLACK, "Metrik Text RGB: " +
    mttxtcolstate); RenderD7::Draw::Text(0, 150, 0.7f, DSEVENBLACK, "Metrik
    Alpha: " + mtcola); RenderD7::Draw::Text(0, 170, 0.7f, DSEVENBLACK, "Metrik
    Text Alpha: " + mttxtcola);*/
    RenderD7::OnScreen(Bottom);
    RenderD7::Draw::Rect(0, 0, 320, 240, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Text(0, 0, 0.7f, RenderD7::Color::Hex("#111111"),
                         "Press B to Get back!");
  } else if (m_state == RFTRACE) {
    RenderD7::OnScreen(Top);
    RenderD7::Draw::Rect(0, 0, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Rect(0, 21, 400, 220, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Text(5, 0, 0.7f, DSEVENWHITE, "RenderD7->FTrace");
    RenderD7::Draw::TextRight(395, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                              RENDERD7VSTRING);
    RenderD7::Draw::Rect(0, 219, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Text(
        5, 220, 0.7f, RenderD7::Color::Hex("#ffffff"),
        "Traces: " + std::to_string(ftrace_index + 1) + "/" +
            std::to_string(RenderD7::Ftrace::rd7_traces.size()));
    RenderD7::Draw::Rect(0, 21, 400, 19, RenderD7::Color::Hex("#aaaaaa"));
    RenderD7::Draw::Text(10, 19, 0.7f, RenderD7::Color::Hex("#000000"),
                         "Function:");
    RenderD7::Draw::TextRight(390, 19, 0.7f, RenderD7::Color::Hex("#000000"),
                              "Time (ms):");

    for (int i = 0; i < 10; i++) {
      if ((i % 2 == 0))
        RenderD7::Draw::Rect(0, 40 + (i)*18, 400, 18,
                             RenderD7::Color::Hex("#cccccc"));
      else
        RenderD7::Draw::Rect(0, 40 + (i)*18, 400, 18,
                             RenderD7::Color::Hex("#bbbbbb"));
    }

    RenderD7::Ftrace::Beg("rd7ft", "display_traces");
    int start_index = ftrace_index < 9 ? 0 : ftrace_index - 9;
    auto it = RenderD7::Ftrace::rd7_traces.begin();
    std::advance(it, start_index);
    int ix = start_index;
    std::string _fkey__ = "0";

    while (ix < (int)RenderD7::Ftrace::rd7_traces.size() &&
           ix < start_index + 10 && it != RenderD7::Ftrace::rd7_traces.end()) {
      if (ix == ftrace_index) {
        _fkey__ = it->first;
        RenderD7::Draw::Rect(0, 40 + (ix - start_index) * 18, 400, 18,
                             RenderD7::Color::Hex("#222222"));
        RenderD7::Draw::Text(
            10, 38 + (ix - start_index) * 18, 0.7f,
            RenderD7::Color::Hex("#ffffff"),
            RenderD7::ShortString(it->second.func_name, 0.7f, 250));
        RenderD7::Draw::TextRight(390, 38 + (ix - start_index) * 18, 0.7f,
                                  RenderD7::Color::Hex("#ffffff"),
                                  RenderD7::MsTimeFmt(it->second.time_of));
      } else {
        RenderD7::Draw::Text(
            10, 38 + (ix - start_index) * 18, 0.7f,
            RenderD7::Color::Hex("#000000"),
            RenderD7::ShortString(it->second.func_name, 0.7f, 250));
        RenderD7::Draw::TextRight(390, 38 + (ix - start_index) * 18, 0.7f,
                                  RenderD7::Color::Hex("#000000"),
                                  RenderD7::MsTimeFmt(it->second.time_of));
      }
      ++it;
      ++ix;
    }

    RenderD7::Ftrace::End("rd7ft", "display_traces");

    RenderD7::OnScreen(Bottom);
    RenderD7::Draw::Rect(0, 0, 320, 240, RenderD7::Color::Hex("#eeeeee"));
    RenderD7::Draw::Rect(0, 0, 400, 21, RenderD7::Color::Hex("#111111"));
    RenderD7::Draw::Text(0, 0, 0.7f, RenderD7::Color::Hex("#ffffff"),
                         "Press B to Get back!");
  }
}
std::string RenderD7::Kbd(int lenght, SwkbdType tp) {
  RenderD7::FrameEnd();
  SwkbdState state;
  char temp[lenght + 1] = {0};

  swkbdInit(&state, tp, 2, lenght);
  swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY,
                     0);
  SwkbdButton ret = swkbdInputText(&state, temp, sizeof(temp));
  temp[lenght] = '\0';
  return (ret == SWKBD_BUTTON_CONFIRM ? temp : "60");
}
void RenderD7::RSettings::Logic(u32 hDown, u32 hHeld, u32 hUp,
                                touchPosition touch) {
  if (m_state == RSETTINGS) {
    mtovlstate = metrikd ? "true" : "false";
    mtscreenstate = mt_screen ? "Bottom" : "Top";
    if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[1])) {
      m_state = RCLOG;
    }
    if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[2])) {
      metrikd = metrikd ? false : true;
      cfgstruct["metrik-settings"]["enableoverlay"] = metrikd ? "1" : "0";
    }
    if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[4])) {
      m_state = RFTRACE;
    }
    /*if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[5])) {
      RenderD7::AddOvl(std::make_unique<RenderD7::DSP_NF>());
    }*/
    if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[6])) {
      m_state = RINFO;
    }
    if (d7_hDown & KEY_TOUCH && RenderD7::touchTObj(d7_touch, buttons[7])) {
      m_state = RSERVICES;
    }
    if (d7_hDown & KEY_B) {
      cfgfile->write(cfgstruct);
      rd7settings = false;
      RenderD7::Scene::Back();
    }
  }
  if (m_state == RINFO) {
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == RSERVICES) {
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == RFTRACE) {
    if (d7_hDown & KEY_DOWN) {
      if (ftrace_index < (int)RenderD7::Ftrace::rd7_traces.size() - 1)
        ftrace_index++;
    }
    if (d7_hDown & KEY_UP) {
      if (ftrace_index > 0)
        ftrace_index--;
    }
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == RCLOG) {
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
    if (d7_hDown & KEY_R)
      if (screen_index < screens)
        screen_index++;
    if (d7_hDown & KEY_L)
      if (screen_index > 0)
        screen_index--;
  }
}

void RenderD7::RSettings::calculate_screens(
    const std::vector<std::string> &lines, int &screen_index, int &screens) {
  int num_lines = lines.size();
  screens = (num_lines + 8) / 9; // round up to nearest multiple of 9
  screen_index = 0;
  screens -= 1;
}

void RenderD7::LoadSettings() {
  RenderD7::Scene::Load(std::make_unique<RenderD7::RSettings>());
}

void RenderD7::AddOvl(std::unique_ptr<RenderD7::Ovl> overlay) {
  overlays.push_back(std::move(overlay));
}

void RenderD7::AddToast(std::unique_ptr<RenderD7::Ovl> overlay) {
  toast_overlays.push_back(std::move(overlay));
}

void RenderD7::FadeOut() {
  if (!waitFade) {
    fadein = true;
    fadealpha = 0;
    waitFade = true;
  }
}

void RenderD7::FadeIn() {
  if (!waitFade) {
    fadeout = true;
    fadealpha = 255;
    waitFade = true;
  }
}

void RenderD7::FadeDisplay() { Npifade(); }

std::string RenderD7::ShortString(std::string in, float size, int maxlen,
                                  C2D_Font font) {
  if (RenderD7::Draw::GetTextWidth(size, in, font) > (float)maxlen) {
    std::string out;
    for (size_t i = 0; i < in.size(); i++) {
      out += in[i];
      if (RenderD7::Draw::GetTextWidth(size, out, font) +
              RenderD7::Draw::GetTextWidth(size, "(...)", font) >
          (float)maxlen) {
        out += "(...)";
        return out;
      }
    }
  }
  return in;
}
