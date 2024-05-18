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
#include <citro2d.h>
#include <citro3d.h>

/// \param Top Tob-Screen Target
extern C3D_RenderTarget *Top;
/// \param TopRight Top-Right-Screen Target (Never atually used)
extern C3D_RenderTarget *TopRight;
/// \param Bottom Bottom-Screen Target
extern C3D_RenderTarget *Bottom;

namespace RenderD7 {
/// @brief Begin Drawing On Specific Screen
/// @param target The Screen Target (Top, Bottom or TopTight)
void OnScreen(C3D_RenderTarget *target);
}  // namespace RenderD7
