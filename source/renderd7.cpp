/**
 *   This file is part of RenderD7
 *   Copyright (C) 2021-2024 NPI-D7, tobid7
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <renderd7/Hid.hpp>  // Integate HidApi
#include <renderd7/Message.hpp>
#include <renderd7/Overlays.hpp>
#include <renderd7/ThemeEditor.hpp>
#include <renderd7/UI7.hpp>
#include <renderd7/renderd7.hpp>
#include <renderd7/renderd7_logo.hpp>

// Config 2
#include <renderd7/external/json.hpp>
#include <renderd7/internal_db.hpp>

// C++ includes
#include <filesystem>
#include <random>

RenderD7::R2Base::Ref rd7i_render2;
RenderD7::LoggerBase::Ref rd7i_glogger;
extern RenderD7::LoggerBase::Ref rd7i_logger;

static void RD7i_ExitHook() {
  C2D_TextBufDelete(rd7i_text_buffer);
  C2D_TextBufDelete(rd7i_d2_dimbuf);
  romfsExit();
}

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
  if (rd7i_fadein) {
    if (rd7i_fadealpha < 255) {
      if ((int)rd7i_fadealpha + 3 > 255) {
        rd7i_fadealpha = 255;
      } else {
        rd7i_fadealpha += 3;
      }
    } else {
      rd7i_fadein = false;
    }
  } else if (rd7i_fadeout) {
    if (rd7i_fadealpha > 0) {
      if ((int)rd7i_fadealpha - 3 < 0) {
        rd7i_fadealpha = 0;
      } else {
        rd7i_fadealpha -= 3;
      }
    } else {
      rd7i_fadeout = false;
    }
  } else {
    if (rd7i_wait_fade) rd7i_wait_fade = false;
    if (rd7i_fade_exit) rd7i_running = false;
    if (rd7i_fade_scene_wait) {
      RenderD7::Scene::scenes.push(std::move(rd7i_fade_scene));
      rd7i_fade_scene_wait = false;
      RenderD7::FadeIn();
    }
    // No fade
  }
  /*if (rd7i_fadein || rd7i_fadeout) {
    RenderD7::R2()->OnScreen(RenderD7::R2Screen_Top);
    RenderD7::R2()->AddRect(R7Vec2(0, 0), R7Vec2(400, 240),
                            ((rd7i_fadealpha << 24) | 0x00000000));
    RenderD7::R2()->OnScreen(RenderD7::R2Screen_Bottom);
    RenderD7::R2()->AddRect(R7Vec2(0, 0), R7Vec2(320, 240),
                            ((rd7i_fadealpha << 24) | 0x00000000));
  }*/
}

void PushSplash() {
  RenderD7::Ftrace::ScopedTrace st("rd7-core", f2s(PushSplash));
  C2D_SpriteSheet sheet;
  sheet = C2D_SpriteSheetLoadFromMem((void *)renderd7_logo, renderd7_logo_size);
  // Display for 2 Sec
  for (int x = 0; x < 120; x++) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(rd7_top, 0xff000000);
    C2D_TargetClear(rd7_bottom, 0xff000000);
    RenderD7::ClearTextBufs();
    C2D_SceneBegin(rd7_top);
    RenderD7::R2()->OnScreen(R2Screen_Top);
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(sheet, 0), 400 / 2 - 300 / 2,
                    240 / 2 - 100 / 2, 0.5);
    C3D_FrameEnd(0);
  }
  C2D_SpriteSheetFree(sheet);
}

void rd7i_init_input() {
  RenderD7::Hid::RegTouchCoords(rd7i_hid_touch_pos);
  RenderD7::Hid::RegKeyDown(d7_hDown);
  RenderD7::Hid::RegKeyHeld(d7_hHeld);
  RenderD7::Hid::RegKeyUp(d7_hUp);
  RenderD7::Hid::RegKeyRepeat(d7_hRepeat);
  RenderD7::Hid::RegKeyEvent("touch", KEY_TOUCH);
  // Default Buttons
  RenderD7::Hid::RegKeyEvent("confirm", KEY_A);
  RenderD7::Hid::RegKeyEvent("cancel", KEY_B);
  RenderD7::Hid::RegKeyEvent("spec2", KEY_X);
  RenderD7::Hid::RegKeyEvent("spec3", KEY_Y);
  RenderD7::Hid::RegKeyEvent("options", KEY_SELECT);
  RenderD7::Hid::RegKeyEvent("spec1", KEY_START);
  // Dpad only
  RenderD7::Hid::RegKeyEvent("dright", KEY_DRIGHT);
  RenderD7::Hid::RegKeyEvent("dleft", KEY_DLEFT);
  RenderD7::Hid::RegKeyEvent("dup", KEY_DUP);
  RenderD7::Hid::RegKeyEvent("ddown", KEY_DDOWN);
  // D + Cpad
  RenderD7::Hid::RegKeyEvent("right", KEY_RIGHT);
  RenderD7::Hid::RegKeyEvent("left", KEY_LEFT);
  RenderD7::Hid::RegKeyEvent("up", KEY_UP);
  RenderD7::Hid::RegKeyEvent("down", KEY_DOWN);
  // Back keys
  RenderD7::Hid::RegKeyEvent("rt1", KEY_R);
  RenderD7::Hid::RegKeyEvent("lt1", KEY_L);
  RenderD7::Hid::RegKeyEvent("rt2", KEY_ZL);
  RenderD7::Hid::RegKeyEvent("lt2", KEY_ZR);
  // Key by their names
  RenderD7::Hid::RegKeyEvent("A", KEY_A);
  RenderD7::Hid::RegKeyEvent("B", KEY_B);
  RenderD7::Hid::RegKeyEvent("X", KEY_X);
  RenderD7::Hid::RegKeyEvent("Y", KEY_Y);
  RenderD7::Hid::RegKeyEvent("L", KEY_L);
  RenderD7::Hid::RegKeyEvent("R", KEY_R);
  RenderD7::Hid::RegKeyEvent("ZR", KEY_ZR);
  RenderD7::Hid::RegKeyEvent("ZL", KEY_ZL);
  RenderD7::Hid::RegKeyEvent("START", KEY_START);
  RenderD7::Hid::RegKeyEvent("SELECT", KEY_SELECT);
  RenderD7::Hid::RegKeyEvent("DUP", KEY_DUP);
  RenderD7::Hid::RegKeyEvent("DDOWN", KEY_DDOWN);
  RenderD7::Hid::RegKeyEvent("DLEFT", KEY_DRIGHT);
  RenderD7::Hid::RegKeyEvent("DRIGHT", KEY_DLEFT);
}

void rd7i_init_config() {
  rd7i_config_path = "sdmc:/RenderD7/Apps/";
  rd7i_config_path += rd7i_app_name;
  std::filesystem::create_directories(rd7i_config_path.c_str());
  std::filesystem::create_directories("sdmc:/RenderD7/Reports");
  bool renew = false;

  if (RenderD7::FS::FileExist(rd7i_config_path + "/config.rc7")) {
    std::fstream cfg_ldr(rd7i_config_path + "/config.rc7", std::ios::in);
    cfg_ldr >> rd7i_config;
    cfg_ldr.close();
    std::string version = rd7i_config["info"]["version"].get<std::string>();
    if (version != CFGVER) renew = true;
  }

  if (!RenderD7::FS::FileExist(rd7i_config_path + "/config.rc7") || renew) {
    if (renew) {
      std::fstream cfg_ldr(rd7i_config_path + "/config.rc7", std::ios::in);
      cfg_ldr >> rd7i_config;
      cfg_ldr.close();
    }
    rd7i_config.clear();
    rd7i_config["info"]["version"] = CFGVER;
    rd7i_config["info"]["renderd7ver"] = RENDERD7VSTRING;
    rd7i_config["metrik-settings"]["show"] = false;
    rd7i_config["metrik-settings"]["Screen"] = true;
    rd7i_config["metrik-settings"]["Text"] = "#ffffffff";
    rd7i_config["metrik-settings"]["Bg"] = "#aa000000";
    rd7i_config["metrik-settings"]["Size"] = 0.7f;
    rd7i_config["internal_logger"]["nowritetxt"] = true;
    std::fstream cfg_wrt(rd7i_config_path + "/config.rc7", std::ios::out);
    cfg_wrt << rd7i_config.dump(4);
    cfg_wrt.close();
  }
  std::fstream cfg_ldr(rd7i_config_path + "/config.rc7", std::ios::in);
  cfg_ldr >> rd7i_config;
  cfg_ldr.close();

  rd7i_metrikd = rd7i_config["metrik-settings"]["show"].get<bool>();
  rd7i_mt_txtSize = rd7i_config["metrik-settings"]["Size"].get<float>();
  rd7i_mt_screen = rd7i_config["metrik-settings"]["Screen"].get<bool>();
  rd7i_lggrf = rd7i_config["internal_logger"]["nowritetxt"].get<bool>();

  if (rd7i_metrikd)
    RenderD7::AddOvl(std::make_unique<RenderD7::Ovl_Metrik>(
        &rd7i_metrikd, &rd7i_mt_screen, &rd7i_mt_color, &rd7i_mt_txtcolor,
        &rd7i_mt_txtSize));
}

void rd7i_init_theme() {
  if (rd7i_config_path == "") {
    rd7i_config_path = "sdmc:/RenderD7/Apps/";
    rd7i_config_path += rd7i_app_name;
  }
  std::string path = rd7i_config_path + "/themes";
  std::filesystem::create_directories(path.c_str());
  bool renew = false;

  if (RenderD7::FS::FileExist(path + "/renderd7.theme")) {
    std::fstream cfg_ldr(path + "/renderd7.theme", std::ios::in);
    nlohmann::json js;
    cfg_ldr >> js;
    cfg_ldr.close();
    std::string version = js["version"].get<std::string>();
    if (version != THEMEVER) renew = true;
  }

  if (!RenderD7::FS::FileExist(path + "/renderd7.theme") || renew) {
    rd7i_amdt = true;
    RenderD7::ThemeActive()->Save(path + "/renderd7.theme");
    rd7i_amdt = false;
  }
}

RenderD7::R2Base::Ref RenderD7::R2() {
  if (!rd7i_render2) {
    RenderD7::Error("Render2 Was Called before being Init!");
    // return schould not be reached then
  }
  return rd7i_render2;
}

RenderD7::LoggerBase::Ref RenderD7::Logger() {
  if (!rd7i_glogger) {
    RenderD7::Error("Logger Was Called before being Init!");
    // return schould not be reached then
  }
  return rd7i_glogger;
}

float RenderD7::GetDeltaTime() { return (float)rd7i_dtm; }

void RenderD7::Init::NdspFirm() {
  if (access("sdmc:/3ds/dspfirm.cdc", F_OK) != -1) {
    ndspInit();
    atexit(ndspExit);
    rd7i_is_ndsp = true;
  } else {
    RenderD7::PushMessage(RenderD7::Message(
        "RenderD7", "dspfirm.cdc not found!\nUnable to play sounds!"));
  }
}

void RenderD7::Scene::doDraw() {
  Ftrace::ScopedTrace st("rd7-core", f2s(Scene::doDraw));
  if (!RenderD7::Scene::scenes.empty()) RenderD7::Scene::scenes.top()->Draw();
}

void RenderD7::Scene::doLogic() {
  Ftrace::ScopedTrace st("rd7-core", f2s(Scene::doLogic));
  if (!RenderD7::Scene::scenes.empty()) RenderD7::Scene::scenes.top()->Logic();
}

void RenderD7::Scene::Load(std::unique_ptr<Scene> scene, bool fade) {
  if (fade) {
    RenderD7::FadeOut();
    rd7i_fade_scene = std::move(scene);
    rd7i_fade_scene_wait = true;
  } else
    Scene::scenes.push(std::move(scene));
}

void RenderD7::Scene::Back() {
  if (RenderD7::Scene::scenes.size() > 0) RenderD7::Scene::scenes.pop();
}

void frameloop() {
  rd7i_frames++;
  rd7i_delta_time = osGetTime() - rd7i_last_time;
  if (rd7i_delta_time >= 1000) {
    rd7i_framerate = rd7i_frames / (rd7i_delta_time / 1000.0f) + 1;
    rd7i_frames = 0;
    rd7i_last_time = osGetTime();
  }
}
float getframerate() { return rd7i_framerate; }

std::string RenderD7::GetFramerate() {
  return (std::to_string((int)rd7i_framerate).substr(0, 2));
}

bool RenderD7::MainLoop() {
  RenderD7::Ftrace::ScopedTrace st("rd7-core", f2s(MainLoop));
  if (!aptMainLoop()) return false;
  // Deltatime
  uint64_t currentTime = svcGetSystemTick();
  rd7i_dtm = ((float)(currentTime / (float)TICKS_PER_MSEC) -
              (float)(rd7i_last_tm / (float)TICKS_PER_MSEC)) /
             1000.f;
  rd7i_time += rd7i_dtm;
  rd7i_last_tm = currentTime;

  hidScanInput();
  d7_hDown = hidKeysDown();
  d7_hUp = hidKeysUp();
  d7_hHeld = hidKeysHeld();
  // Inofficial
  d7_hRepeat = hidKeysDownRepeat();
  hidTouchRead(&d7_touch);
  Hid::Update();
  rd7i_hid_touch_pos = R7Vec2(d7_touch.px, d7_touch.py);

  RenderD7::ClearTextBufs();
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

  C2D_TargetClear(rd7_top, C2D_Color32(0, 0, 0, 0));
  C2D_TargetClear(rd7_bottom, C2D_Color32(0, 0, 0, 0));
  frameloop();
  if (rd7_enable_scene_system) {
    RenderD7::Scene::doDraw();
    RenderD7::Scene::doLogic();
  }
  return rd7i_running;
}

void RenderD7::ClearTextBufs(void) { C2D_TextBufClear(rd7i_text_buffer); }

void RenderD7::Init::Graphics() {
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init((size_t)rd7_max_objects);
  C2D_Prepare();
  rd7_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  rd7_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  rd7_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  rd7i_text_buffer = C2D_TextBufNew(4096);
  rd7i_d2_dimbuf = C2D_TextBufNew(4096);
  rd7i_base_font = C2D_FontLoadSystem(CFG_REGION_USA);
  rd7i_render2 = R2Base::New();
}

Result RenderD7::Init::Main(std::string app_name) {
  RenderD7::Ftrace::ScopedTrace st("rd7-core", f2s(Init::Main));
  rd7i_app_name = app_name;
  rd7i_logger = LoggerBase::New();
  rd7i_glogger = LoggerBase::New();

  gfxInitDefault();
  atexit(gfxExit);
  // Speedup
  osSetSpeedupEnable(true);
  // consoleInit(GFX_TOP, NULL);
  cfguInit();
  atexit(cfguExit);
  CFGU_SecureInfoGetRegion(&rd7i_system_region);
  CFGU_GetSystemModel(&rd7i_console_model);

  aptInit();
  atexit(aptExit);
  romfsInit();

  rd7i_init_config();
  _rd7i_logger()->Init("renderd7", rd7i_lggrf);

  rd7i_active_theme = Theme::New();
  rd7i_active_theme->Default();

  auto ret = rd7i_soc_init();
  if (ret) {
    rd7i_logger->Write("Failed to Init Soc!");
    RenderD7::PushMessage("RenderD7", "Failed to\nInit Soc!");
  } else {
    atexit(rd7i_soc_deinit);
  }

  if (R_SUCCEEDED(amInit())) {
    atexit(amExit);
    rd7i_is_am_init = true;
  }

  Hardware::Initialisize();

  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  atexit(C3D_Fini);
  C2D_Init((size_t)rd7_max_objects);
  atexit(C2D_Fini);
  atexit(RD7i_ExitHook);
  C2D_Prepare();
  rd7_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  rd7_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  rd7_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  rd7i_text_buffer = C2D_TextBufNew(4096);
  rd7i_d2_dimbuf = C2D_TextBufNew(4096);
  rd7i_base_font = C2D_FontLoadSystem(CFG_REGION_USA);
  rd7i_render2 = R2Base::New();

  rd7i_graphics_on = true;
  rd7i_last_tm = svcGetSystemTick();
  if (rd7_do_splash) PushSplash();

  rd7i_init_input();
  rd7i_init_theme();
  UI7::Init();
  atexit(UI7::Deinit);
  rd7i_running = true;
  return 0;
}

Result RenderD7::Init::Minimal(std::string app_name) {
  RenderD7::Ftrace::ScopedTrace st("rd7-core", f2s(Init::Minimal));
  rd7i_app_name = app_name;
  rd7i_logger = LoggerBase::New();
  rd7i_glogger = LoggerBase::New();

  gfxInitDefault();
  atexit(gfxExit);
  romfsInit();

  rd7i_init_config();
  _rd7i_logger()->Init("renderd7", rd7i_lggrf);

  rd7i_active_theme = Theme::New();
  rd7i_active_theme->Default();

  auto ret = rd7i_soc_init();
  if (ret) {
    rd7i_logger->Write("Failed to Init Soc!");
    RenderD7::PushMessage("RenderD7", "Failed to\nInit Soc!");
  } else {
    atexit(rd7i_soc_deinit);
  }

  if (R_SUCCEEDED(amInit())) {
    atexit(amExit);
    rd7i_is_am_init = true;
  }

  Hardware::Initialisize();

  osSetSpeedupEnable(true);
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  atexit(C3D_Fini);
  C2D_Init((size_t)rd7_max_objects);
  atexit(C2D_Fini);
  atexit(RD7i_ExitHook);
  C2D_Prepare();
  rd7_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  rd7_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  rd7_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  rd7i_text_buffer = C2D_TextBufNew(4096);
  rd7i_d2_dimbuf = C2D_TextBufNew(4096);
  rd7i_base_font = C2D_FontLoadSystem(CFG_REGION_USA);
  rd7i_render2 = R2Base::New();

  rd7i_graphics_on = true;
  if (rd7_do_splash) PushSplash();

  // Check if citra
  s64 citracheck = 0;
  svcGetSystemInfo(&citracheck, 0x20000, 0);
  rd7i_is_citra = citracheck ? true : false;

  rd7i_init_input();
  rd7i_init_theme();
  UI7::Init();
  atexit(UI7::Deinit);
  rd7i_running = true;
  return 0;
}

Result RenderD7::Init::Reload() {
  rd7i_graphics_on = false;
  C2D_TextBufDelete(rd7i_text_buffer);
  rd7i_render2 = nullptr;  // Delete Render2
  C2D_Fini();
  C3D_Fini();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init((size_t)rd7_max_objects);
  C2D_Prepare();
  rd7_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  rd7_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  rd7_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  rd7i_text_buffer = C2D_TextBufNew(4096);
  rd7i_base_font = C2D_FontLoadSystem(CFG_REGION_USA);
  rd7i_render2 = R2Base::New();
  rd7i_graphics_on = true;

  return 0;
}

void RenderD7::ExitApp() {
  if (rd7i_wait_fade) {
    rd7i_fade_exit = true;
  } else
    rd7i_running = false;
}

int RenderD7::GetRandomInt(int b, int e) {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(b, e);
  int r = distribution(generator);
  return r;
}

bool RenderD7::FS::FileExist(const std::string &path) {
  return std::filesystem::exists(path) &&
         std::filesystem::is_regular_file(path);
}

int RenderD7::GetFps() { return (int)rd7i_framerate; }

bool RenderD7::IsNdspInit() { return rd7i_is_ndsp; }

void OvlHandler() {
  RenderD7::Ftrace::ScopedTrace st("rd7-core", f2s(OvlHandler));
  for (size_t i = 0; i < rd7i_overlays.size(); i++) {
    rd7i_overlays[i]->Draw();
    rd7i_overlays[i]->Logic();
    if (rd7i_overlays[i]->IsKilled())
      rd7i_overlays.erase(rd7i_overlays.begin() + i);
  }
}

void RenderD7::FrameEnd() {
  Ftrace::ScopedTrace st("rd7-core", f2s(FrameEnd));
  C3D_FrameBegin(2);
  if (!rd7_enable_scene_system && rd7i_settings) {
    RenderD7::Scene::doDraw();
    RenderD7::Scene::doLogic();
  }
  UI7::Update();
  UI7::Debug();
  RenderD7::ProcessMessages();
  OvlHandler();
  Npifade();
  R2()->Process();
  C3D_FrameEnd(0);
}

RenderD7::RSettings::RSettings() {
  // RenderD7 Settings is designed for
  // System Font
  R2()->DefaultFont();
  tmp_txt = R2()->GetTextSize();
  R2()->DefaultTextSize();
  RenderD7::FadeIn();
  std::fstream cfg_ldr(rd7i_config_path + "/config.rc7", std::ios::in);
  cfg_ldr >> rd7i_config;
  cfg_ldr.close();
  rd7i_settings = true;
  statemtold = rd7i_metrikd;
  stateftold = rd7i_ftraced;
}

RenderD7::RSettings::~RSettings() { R2()->SetTextSize(tmp_txt); }

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
    RenderD7::R2()->OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("RenderD7 -> Settings")) {
      UI7::SetCursorPos(R7Vec2(395, 2));
      UI7::Label(RENDERD7VSTRING, RD7TextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Config Version: " + std::string(CFGVER));
      UI7::Label("App: " + rd7i_app_name);
      UI7::Label("RenderD7: " + std::string(RENDERD7VSTRING));
      UI7::Label("Citra: " + std::string(rd7i_is_citra ? "true" : "false"));
      UI7::Label("Current: " + std::to_string(RenderD7::Memory::GetCurrent()) +
                 "b");
      UI7::Label("Delta: " + std::to_string(RenderD7::GetDeltaTime()));
      UI7::Label("Kbd test: " + kbd_test);
      UI7::EndMenu();
    }
    RenderD7::R2()->OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!")) {
      if (UI7::Button("FTrace")) {
        shared_request[0x00000001] = RFTRACE;
      }
      if (UI7::Button("UI7")) {
        shared_request[0x00000001] = RUI7;
      }
      if (UI7::Button("Overlays")) {
        shared_request[0x00000001] = ROVERLAYS;
      }
      if (UI7::Button("IDB")) {
        shared_request[0x00000001] = RIDB;
      }
      if (UI7::Button("ThemeEditor")) {
        RenderD7::LoadThemeEditor();
      }
      if (UI7::Button("Logs")) {
        shared_request[0x00000001] = RLOGS;
      }
      UI7::SameLine();
      UI7::Checkbox("No File", rd7i_lggrf);
      if (UI7::Button("Back")) {
        shared_request[0x00000002] = 1U;
      }
      if (UI7::Button("Keyboard")) {
        shared_request[0x00000003] = 1U;
      }
      UI7::EndMenu();
    }

  } else if (m_state == RIDB) {
    RenderD7::R2()->OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("RenderD7 -> Debugger")) {
      UI7::SetCursorPos(R7Vec2(395, 2));
      UI7::Label(RENDERD7VSTRING, RD7TextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Server Running: " +
                 std::string(rd7i_idb_running ? "true" : "false"));
      UI7::EndMenu();
    }
    RenderD7::R2()->OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!")) {
      if (UI7::Button("Start Server")) {
        RenderD7::IDB::Start();
      }
      UI7::SameLine();
      if (UI7::Button("Stop Server")) {
        RenderD7::IDB::Stop();
      }
      UI7::SameLine();
      if (UI7::Button("Restart Server")) {
        RenderD7::IDB::Restart();
      }

      UI7::EndMenu();
    }

  } else if (m_state == RFTRACE) {
    RenderD7::R2()->OnScreen(R2Screen_Top);
    // Draw Top Screen Into Background DrawList
    UI7::GetBackgroundList()->AddRectangle(R7Vec2(0, 0), R7Vec2(400, 240),
                                           RD7Color_Background);
    UI7::GetBackgroundList()->AddRectangle(R7Vec2(0, 0), R7Vec2(400, 20),
                                           RD7Color_Header);
    UI7::GetBackgroundList()->AddText(
        R7Vec2(5, 2), "RenderD7 -> FTrace",
        RenderD7::ThemeActive()->AutoText(RD7Color_Header));
    UI7::GetBackgroundList()->AddText(
        R7Vec2(395, 2), RENDERD7VSTRING,
        RenderD7::ThemeActive()->AutoText(RD7Color_Header),
        RD7TextFlags_AlignRight);
    UI7::GetBackgroundList()->AddRectangle(
        R7Vec2(0, 220), R7Vec2(400, 20),
        RenderD7::ThemeActive()->Get(RD7Color_Header));
    UI7::GetBackgroundList()->AddText(
        R7Vec2(5, 222),
        "Traces: " + std::to_string(ftrace_index + 1) + "/" +
            std::to_string(RenderD7::Ftrace::rd7_traces.size()),
        RenderD7::ThemeActive()->AutoText(RD7Color_Header));
    UI7::GetBackgroundList()->AddRectangle(R7Vec2(0, 20), R7Vec2(400, 20),
                                           RD7Color_TextDisabled);
    UI7::GetBackgroundList()->AddText(
        R7Vec2(5, 22),
        "Function:", RenderD7::ThemeActive()->AutoText(RD7Color_TextDisabled));
    UI7::GetBackgroundList()->AddText(
        R7Vec2(395, 22),
        "Time (ms):", RenderD7::ThemeActive()->AutoText(RD7Color_TextDisabled),
        RD7TextFlags_AlignRight);

    // List Bg
    for (int i = 0; i < 12; i++) {
      if ((i % 2 == 0))
        UI7::GetBackgroundList()->AddRectangle(R7Vec2(0, 40 + (i) * 15),
                                               R7Vec2(400, 15), RD7Color_List0);
      else
        UI7::GetBackgroundList()->AddRectangle(R7Vec2(0, 40 + (i) * 15),
                                               R7Vec2(400, 15), RD7Color_List1);
    }

    RenderD7::Ftrace::Beg("rd7ft", "display_traces");
    int start_index = ftrace_index < 11 ? 0 : ftrace_index - 11;
    auto it = RenderD7::Ftrace::rd7_traces.begin();
    std::advance(it, start_index);
    int ix = start_index;
    std::string _fkey__ = "0";

    while (ix < (int)RenderD7::Ftrace::rd7_traces.size() &&
           ix < start_index + 10 && it != RenderD7::Ftrace::rd7_traces.end()) {
      if (ix == ftrace_index) {
        _fkey__ = it->first;
        UI7::GetBackgroundList()->AddRectangle(
            R7Vec2(0, 40 + (ix - start_index) * 15), R7Vec2(400, 15),
            RD7Color_Selector);
      }
      auto clr = ix == ftrace_index
                     ? RD7Color_Selector
                     : (ix % 2 == 0 ? RD7Color_List0 : RD7Color_List1);
      UI7::GetBackgroundList()->AddText(R7Vec2(5, 40 + (ix - start_index) * 15),
                                        it->second.func_name,
                                        RenderD7::ThemeActive()->AutoText(clr));
      UI7::GetBackgroundList()->AddText(
          R7Vec2(395, 40 + (ix - start_index) * 15),
          RenderD7::MsTimeFmt(it->second.time_of),
          RenderD7::ThemeActive()->AutoText(clr), RD7TextFlags_AlignRight);
      ++it;
      ++ix;
    }

    RenderD7::Ftrace::End("rd7ft", "display_traces");

    RenderD7::R2()->OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!")) {
      auto jt = RenderD7::Ftrace::rd7_traces.begin();
      std::advance(jt, ftrace_index);
      UI7::Label("Group: " + jt->second.group);
      UI7::Label("Function: " + jt->second.func_name);
      UI7::Checkbox("In Overlay", jt->second.is_ovl);
      UI7::Label("Time: " + RenderD7::MsTimeFmt(jt->second.time_of));
      UI7::Label("Max: " + RenderD7::MsTimeFmt(jt->second.time_ofm));
      UI7::Label("TS: " + std::to_string(jt->second.time_start));
      UI7::Label("TE: " + std::to_string(jt->second.time_end));
      UI7::Label("SVC_Stk: " + std::to_string(svcGetSystemTick()));
      UI7::EndMenu();
    }
  } else if (m_state == RUI7) {
    RenderD7::R2()->OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("RenderD7 -> UI7")) {
      UI7::SetCursorPos(R7Vec2(395, 2));
      UI7::Label(RENDERD7VSTRING, RD7TextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Time: " + std::to_string(UI7::GetTime()));
      UI7::Label("Delta: " + std::to_string(UI7::GetDeltaTime() * 1000.f));
      UI7::Label("Hid Down Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Down)));
      UI7::Label("Hid Held Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Held)));
      UI7::Label("Hid Up Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Up)));
      UI7::Label("Touch Pos: " + std::to_string(Hid::GetTouchPosition().x) +
                 ", " + std::to_string(Hid::GetTouchPosition().y));
      UI7::Label(
          "Touch Last Pos: " + std::to_string(Hid::GetLastTouchPosition().x) +
          ", " + std::to_string(Hid::GetLastTouchPosition().y));
      UI7::Label(
          "Touch Down Pos: " + std::to_string(Hid::GetTouchDownPosition().x) +
          ", " + std::to_string(Hid::GetTouchDownPosition().y));
      UI7::EndMenu();
    }

    RenderD7::R2()->OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!", R7Vec2(),
                       UI7MenuFlags_Scrolling)) {
      if (UI7::Button("Go back")) {
        /// Request a state switch to state RSETTINGS
        shared_request[0x00000001] = RSETTINGS;
      }
      UI7::Checkbox("Debug", UI7::IsDebugging());
      UI7::Checkbox("ShowMenuInfo", UI7::DebugMenu());
      UI7::EndMenu();
    }
  } else if (m_state == ROVERLAYS) {
    RenderD7::R2()->OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("RenderD7 -> Overlays")) {
      UI7::SetCursorPos(R7Vec2(395, 2));
      UI7::Label(RENDERD7VSTRING, RD7TextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Metrik Overlay: " + mtovlstate);
      UI7::Label("Metrik Screen: " + mtscreenstate);
      UI7::EndMenu();
    }

    RenderD7::R2()->OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!")) {
      UI7::Label("Metrik:");
      UI7::Checkbox("Enable Overlay", rd7i_metrikd);
      UI7::Checkbox("Bottom Screen", rd7i_mt_screen);
      UI7::Label("FTrace:");
      UI7::Checkbox("Enable Overlay", rd7i_ftraced);
      UI7::SetCursorPos(R7Vec2(5, 215));
      if (UI7::Button("Go back")) {
        /// Request a state switch to state RSETTINGS
        shared_request[0x00000001] = RSETTINGS;
      }
      UI7::EndMenu();
    }
  } else if (m_state == RLOGS) {
    RenderD7::R2()->OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("RenderD7 -> Logs")) {
      UI7::SetCursorPos(R7Vec2(395, 2));
      UI7::Label(RENDERD7VSTRING, RD7TextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::EndMenu();
    }

    RenderD7::R2()->OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!", R7Vec2(),
                       UI7MenuFlags_Scrolling)) {
      for (auto &it : rd7i_logger->Lines()) UI7::Label(it, RD7TextFlags_Wrap);
      UI7::EndMenu();
    }
  }
}

void RenderD7::RSettings::Logic() {
  /// Requests
  for (const auto &it : shared_request) {
    if (it.first == 0x00000001) {
      m_state = (RState)it.second;
    } else if (it.first == 0x00000002) {
      if (it.second) {
        std::fstream cfg_wrt(rd7i_config_path + "/config.rc7", std::ios::out);
        rd7i_config["metrik-settings"]["enableoverlay"] = rd7i_metrikd;
        rd7i_config["metrik-settings"]["Screen"] = rd7i_mt_screen;
        rd7i_config["internal_logger"]["nowritetxt"] = rd7i_lggrf;
        cfg_wrt << rd7i_config.dump(4);
        cfg_wrt.close();
        rd7i_settings = false;
        Hid::Clear();
        RenderD7::Scene::Back();
        // Instant break logic or it will crash
        return;
      }
    } else if (it.first == 0x00000003) {
      if (it.second)
        RenderD7::AddOvl(std::make_unique<Ovl_Keyboard>(kbd_test, kbd_state));
    }
  }
  /// Clear if handled
  shared_request.clear();

  if (statemtold != rd7i_metrikd && rd7i_metrikd == true)
    RenderD7::AddOvl(std::make_unique<Ovl_Metrik>(
        &rd7i_metrikd, &rd7i_mt_screen, &rd7i_mt_color, &rd7i_mt_txtcolor,
        &rd7i_mt_txtSize));
  statemtold = rd7i_metrikd;
  if (stateftold != rd7i_ftraced && rd7i_ftraced == true)
    RenderD7::AddOvl(std::make_unique<Ovl_Ftrace>(&rd7i_ftraced));
  stateftold = rd7i_ftraced;

  if (m_state == RSETTINGS) {
    if (d7_hDown & KEY_B) {
      std::fstream cfg_wrt(rd7i_config_path + "/config.rc7", std::ios::out);
      rd7i_config["metrik-settings"]["enableoverlay"] = rd7i_metrikd;
      rd7i_config["metrik-settings"]["Screen"] = rd7i_mt_screen;
      rd7i_config["internal_logger"]["nowritetxt"] = rd7i_lggrf;
      cfg_wrt << rd7i_config.dump(4);
      cfg_wrt.close();
      rd7i_settings = false;
      Hid::Clear();
      RenderD7::Scene::Back();
    }
  }
  if (m_state == RUI7) {
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == ROVERLAYS) {
    mtovlstate = rd7i_metrikd ? "true" : "false";
    mtscreenstate = rd7i_mt_screen ? "Bottom" : "Top";
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == RIDB || m_state == RLOGS) {
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
      if (ftrace_index > 0) ftrace_index--;
    }
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
  }
}

void RenderD7::LoadSettings() {
  if (!rd7i_settings)
    RenderD7::Scene::Load(std::make_unique<RenderD7::RSettings>());
}

void RenderD7::LoadThemeEditor() {
  RenderD7::Scene::Load(std::make_unique<RenderD7::ThemeEditor>());
}

void RenderD7::AddOvl(std::unique_ptr<RenderD7::Ovl> overlay) {
  rd7i_overlays.push_back(std::move(overlay));
}

void RenderD7::FadeOut() {
  if (!rd7i_wait_fade) {
    rd7i_fadein = true;
    rd7i_fadealpha = 0;
    rd7i_wait_fade = true;
  }
}

void RenderD7::FadeIn() {
  if (!rd7i_wait_fade) {
    rd7i_fadeout = true;
    rd7i_fadealpha = 255;
    rd7i_wait_fade = true;
  }
}

void RenderD7::FadeDisplay() { Npifade(); }

float RenderD7::GetTime() { return rd7i_time; }

std::string RenderD7::GetAppDirectory() {
  return "sdmc:/RenderD7/Apps/" + rd7i_app_name;
}
