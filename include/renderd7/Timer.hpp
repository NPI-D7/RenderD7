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

#include <3ds.h>

#include <renderd7/smart_ctor.hpp>

namespace RenderD7 {
class Timer {
 public:
  Timer(bool autostart = true);
  ~Timer() {}
  RD7_SMART_CTOR(Timer)
  void Reset();
  void Tick();
  void Pause();
  void Resume();
  float Get();
  float GetLive();
  bool Running();

 private:
  uint64_t last = 0;
  uint64_t current = 0;
  bool is_running = false;
};
}  // namespace RenderD7