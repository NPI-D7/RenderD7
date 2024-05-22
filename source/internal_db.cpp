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

#include <arpa/inet.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <renderd7/FileSystem.hpp>
#include <renderd7/external/json.hpp>
#include <renderd7/internal_db.hpp>
#include <renderd7/renderd7.hpp>

/// Base ///
static RenderD7::Thread rd7i_idb_server;  // Protected
std::string rd7i_app_name;
std::string rd7i_config_path;
nlohmann::json rd7i_config;
u8 rd7i_console_model = 0;
u8 rd7i_system_region = CFG_REGION_USA;
bool rd7i_is_citra = false;
bool rd7i_settings = false;
R7Vec2 rd7i_hid_touch_pos;
C2D_TextBuf rd7i_text_buffer;
C2D_Font rd7i_base_font;
C2D_TextBuf rd7i_d2_dimbuf;
bool rd7i_is_ndsp = false;
bool rd7i_running = false;
std::stack<std::unique_ptr<RenderD7::Scene>> RenderD7::Scene::scenes;
std::unique_ptr<RenderD7::Scene> rd7i_fade_scene;
std::vector<std::unique_ptr<RenderD7::Ovl>> rd7i_overlays;
unsigned int rd7i_frames = 0;
u64 rd7i_last_time = 0;
float rd7i_framerate = 0.0f;
u32 rd7i_mt_color = 0xaa000000;
u32 rd7i_mt_txtcolor = 0xbbffffff;
bool rd7i_mt_screen;
float rd7i_mt_txtSize;
bool rd7i_metrikd = false;
bool rd7i_ftraced = false;
bool rd7i_current_screen = false;
u64 rd7i_delta_time;
u64 rd7i_last_tm;
float rd7i_dtm;
float rd7i_time;
bool rd7i_fadeout = false, rd7i_fadein = false, rd7i_fadeout2 = false,
     rd7i_fadein2 = false;
int rd7i_fadealpha = 0;
int rd7i_fadecolor = 0;
bool rd7i_wait_fade = false;
bool rd7i_fade_exit = false;
bool rd7i_fade_scene_wait = false;
bool rd7i_idb_running = false;
bool rd7i_graphics_on = false;
float rd7_draw2_tsm = 1.2f;
bool rd7i_amdt = false;
void *rd7i_soc_buf = nullptr;

/// Global ///
// Outdated HidApi (HidV2Patched)
u32 d7_hDown;
u32 d7_hHeld;
u32 d7_hUp;
u32 d7_hRepeat;  // Inofficial lol
touchPosition d7_touch;

// Outdated Screens
C3D_RenderTarget *Top;
C3D_RenderTarget *TopRight;
C3D_RenderTarget *Bottom;

// Modern Global Api
int rd7_max_objects = C2D_DEFAULT_MAX_OBJECTS;
bool rd7_do_splash = false;
bool rd7_enable_scene_system = true;
bool rd7_debugging = false;
C3D_RenderTarget *rd7_top;
C3D_RenderTarget *rd7_top_right;
C3D_RenderTarget *rd7_bottom;

RenderD7::Net::Error rd7i_soc_init() {
  if (rd7i_soc_buf != nullptr) {
    return 0;
  }
  rd7i_soc_buf = memalign(0x1000, 0x100000);
  if (!rd7i_soc_buf) {
    return RenderD7::Net::Error_Memory;
  }
  Result ret = socInit((u32 *)rd7i_soc_buf, 0x100000);
  if (R_FAILED(ret)) {
    free(rd7i_soc_buf);
    return ((static_cast<RenderD7::Net::Error>(ret) << 32) |
            static_cast<RenderD7::Net::Error>(RenderD7::Net::Error_CtrStatus));
  }
  return 0;
}

void rd7i_soc_deinit() {
  if (rd7i_soc_buf) {
    socExit();
    free(rd7i_soc_buf);
  }
  rd7i_soc_buf = nullptr;
}

class Logger {
 public:
  Logger() = default;
  virtual ~Logger() = default;

  static void log(const std::string &prefix, const std::string &message) {
    std::cout << "[" << prefix << "]: " << message << std::endl;
  }
};

#define rd7_err(x) Logger::log("ERROR", x)
#define rd7_wrn(x) Logger::log("WARNING", x)

class tcp_server {
 public:
  tcp_server(const std::string &ip, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      rd7_err("Unable to create socket!");
      return;
    }
    server.sin_family = AF_INET;
    server.sin_port = port;
    server.sin_addr.s_addr = inet_addr(ip.c_str());

    e = bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (e == -1) {
      rd7_err("Unable to bind!");
      return;
    }
    e = listen(sockfd, 10);
    if (e == 0) {
      printf("[+]Listening...\n");
    } else {
      rd7_err("Error in Binding");
      return;
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
    std::cout << "Connected" << std::endl;
  }
  ~tcp_server() {
    // Nothing here
  }

  bool reconnect() {
    close(new_sock);
    new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
    return true;
  }

  size_t snd(void *ptr, size_t size) { return send(new_sock, ptr, size, 0); }

  size_t rec(void *ptr, size_t size) { return recv(new_sock, ptr, size, 0); }

 private:
  int e, sockfd, new_sock;
  struct sockaddr_in server, new_addr;
  socklen_t addr_size;
};

#define stupid(x) &x, sizeof(x)
#define rd7i_reacttion(x)     \
  {                           \
    int code = x;             \
    server.snd(stupid(code)); \
  }

struct pak32 {
  pak32() {}
  pak32(const std::string &n0, float n1, unsigned char n2, unsigned char n3,
        bool n4, bool n5, bool n6, float n7, float n8, float n9, float n10,
        unsigned int n11, unsigned int n12, unsigned int n13,
        unsigned int n14) {
    magic = 0x44772277;
    for (int i = 0; i < 64; i++) app_name[i] = (char)0;
    int l = n0.length();
    if (l > 64) l = 64;
    for (int i = 0; i < l; i++) app_name[i] = n0[i];
    framerate = n1;
    console_model = n2;
    system_region = n3;
    is_citra = n4;
    ndsp_support = n5;
    in_settings = n6;
    deltatime = n7;
    rtime = n8;
    cpu = n9;
    gpu = n10;
    mem_alloc = n11;
    mem_dalloc = n12;
    mem_ialloc = n13;
    tbs = n14;
  }
  uint32_t magic;
  char app_name[64];
  float framerate;
  unsigned char console_model;
  unsigned char system_region;
  bool is_citra;
  bool ndsp_support;
  bool in_settings;
  float deltatime;
  float rtime;
  float cpu;
  float gpu;
  unsigned int mem_alloc;
  unsigned int mem_dalloc;
  unsigned int mem_ialloc;
  unsigned int tbs;
};

static bool rd7i_idb_fp = false;

void KillIdbServer() {
  rd7i_idb_fp = true;
  rd7i_idb_server.join(100);
  rd7i_soc_deinit();
}

void ServerThread(RenderD7::Parameter param) {
  if (rd7i_soc_init()) return;
  rd7i_idb_running = true;
  rd7i_idb_fp = false;
  atexit(KillIdbServer);
  tcp_server server("0.0.0.0", 4727);
  int cmd = 0;
  while (!rd7i_idb_fp) {
    size_t r = server.rec(&cmd, sizeof(int));
    if (r == 0) {
      server.reconnect();
    }
    if (cmd == 1) {
      rd7i_reacttion(1);
      auto pak = pak32(
          rd7i_app_name, rd7i_framerate, rd7i_console_model, rd7i_system_region,
          rd7i_is_citra, rd7i_is_ndsp, rd7i_settings, rd7i_dtm, rd7i_time,
          C3D_GetProcessingTime(), C3D_GetDrawingTime(),
          RenderD7::Memory::GetTotalAllocated(),
          RenderD7::Memory::GetTotalFreed(), RenderD7::Memory::GetCurrent(),
          C2D_TextBufGetNumGlyphs(rd7i_text_buffer));
      server.snd(stupid(pak));
    } else if (cmd == 2) {
      rd7i_reacttion(2);
      std::vector<RenderD7::FileSystem::Entry> el =
          RenderD7::FileSystem::GetDirContent("sdmc:/");
      size_t buf = el.size() * sizeof(RenderD7::FileSystem::Entry);
      server.snd(stupid(buf));
      server.snd(el.data(), buf);

    } else if (cmd == 3) {
      rd7i_reacttion(3);
      RenderD7::ExitApp();
      rd7i_idb_fp = true;
    } else {
      rd7i_reacttion(1234);
    }
  }
  rd7i_idb_running = false;
}

namespace RenderD7 {
namespace IDB {
void Start() {
  if (rd7i_idb_running) return;
  rd7i_idb_server.initialize(ServerThread);
  rd7i_idb_server.start(true);
}
void Stop() {
  if (!rd7i_idb_running) return;
  rd7i_idb_fp = true;
  KillIdbServer();
}
void Restart() {
  Stop();
  Start();
}
}  // namespace IDB
}  // namespace RenderD7