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

// WARNING
// THIS IS BETA STUFF
// ITS MAKE LIKE EXTERNAL BUT
// FOR RENDERD7 ITS INTEGRATED

#pragma once

#include <renderd7/R7Vec.hpp>
#include <string>

namespace RenderD7 {
namespace Hid {
enum Actions {
  Down = 0,
  Held = 1,
  Up = 2,
  DownRepeat = 3,
};
// Register Functions
// Register Current state values
void RegKeyDown(uint32_t &key_down);
void RegKeyHeld(uint32_t &key_held);
void RegKeyUp(uint32_t &key_up);
void RegKeyRepeat(uint32_t &repeat);
void RegTouchCoords(R7Vec2 &touch_pos);
// Not Corectly Implemented Yet
void RegAnalog1Movement(R7Vec2 &movement);
void RegAnalog2Movement(R7Vec2 &movement);
// Register Keys
void RegKeyEvent(const std::string &event, uint32_t key);
// KeyEvents
bool IsEvent(const std::string &event, Actions action);
R7Vec2 GetTouchPosition();
R7Vec2 GetLastTouchPosition();
void Update();
// Lock/Unlock Input api for example for Keyboard
void Lock();
void Unlock();
void Clear();
}  // namespace Hid
}  // namespace RenderD7