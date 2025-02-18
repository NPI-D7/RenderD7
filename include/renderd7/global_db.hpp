#pragma once

#include <renderd7/renderd7.hpp>
#include <renderd7/external/json.hpp>

// Outdated HidApi (HidV2Patched)
extern u32 d7_hDown;
extern u32 d7_hHeld;
extern u32 d7_hUp;
extern u32 d7_hRepeat;  // Inofficial lol
extern touchPosition d7_touch;

// Outdated Screens
extern C3D_RenderTarget *Top;
extern C3D_RenderTarget *TopRight;
extern C3D_RenderTarget *Bottom;

// Modern Global Api
extern int rd7_max_objects;
extern bool rd7_do_splash;
extern bool rd7_enable_scene_system;
extern bool rd7_debugging;
extern C3D_RenderTarget *rd7_top;
extern C3D_RenderTarget *rd7_top_right;
extern C3D_RenderTarget *rd7_bottom;

// Draw2
extern float rd7_draw2_tsm;