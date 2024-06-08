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

#pragma once

#include <renderd7/external/json.hpp>
#include <renderd7/renderd7.hpp>

namespace RenderD7 {
namespace IDB {
void Start();
void Stop();
void Restart();
}  // namespace IDB
}  // namespace RenderD7

// Outdated HidApi (HidV2Patched)
extern u32 d7_hDown;
extern u32 d7_hHeld;
extern u32 d7_hUp;
extern u32 d7_hRepeat;  // Inofficial lol
extern touchPosition d7_touch;

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