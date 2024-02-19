#pragma once
#include <renderd7/external/json.hpp>
#include <renderd7/global_db.hpp>
#include <renderd7/renderd7.hpp>

#define CFGVER "0"
#define THEMEVER "0"

#ifndef V_RD7BTIME
#define V_RD7BTIME "SUBMODULE"
#endif
#ifndef V_RD7CSTRING
#define V_RD7CSTRING "SUBMODULE"
#endif

// Base
extern std::string rd7i_app_name;
extern std::string rd7i_config_path;
extern nlohmann::json rd7i_config;
extern u8 rd7i_console_model;
extern u8 rd7i_system_region;
extern bool rd7i_is_citra;
extern bool rd7i_settings;
extern R7Vec2 rd7i_hid_touch_pos;
extern C2D_TextBuf rd7i_text_buffer;
extern C2D_TextBuf rd7i_d2_dimbuf;
extern C2D_Font rd7i_base_font;
extern bool rd7i_is_ndsp;
extern bool rd7i_running;
extern std::unique_ptr<RenderD7::Scene> rd7i_fade_scene;
extern std::vector<std::unique_ptr<RenderD7::Ovl>> rd7i_overlays;
extern unsigned int rd7i_frames;
extern u64 rd7i_last_time;
extern float rd7i_framerate;
extern u32 rd7i_mt_color;
extern u32 rd7i_mt_txtcolor;
extern bool rd7i_mt_screen;
extern float rd7i_mt_txtSize;
extern bool rd7i_metrikd;
extern bool rd7i_ftraced;
extern bool rd7i_current_screen;
extern u64 rd7i_delta_time;
extern u64 rd7i_last_tm;
extern float rd7i_dtm;
extern float rd7i_time;
extern bool rd7i_fadeout;
extern bool rd7i_fadein;
extern bool rd7i_fadeout2;
extern bool rd7i_fadein2;
extern int rd7i_fadealpha;
extern int rd7i_fadecolor;
extern bool rd7i_wait_fade;
extern bool rd7i_fade_exit;
extern bool rd7i_fade_scene_wait;
extern bool rd7i_idb_running;
extern bool rd7i_graphics_on;
extern bool rd7i_amdt;