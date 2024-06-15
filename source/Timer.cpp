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

#include <renderd7/Timer.hpp>

// Ticks per MSEC
#define TPMS 268111.856

namespace RenderD7 {
Timer::Timer(bool autostart) {
  if (autostart) is_running = true;
  last = svcGetSystemTick();
  current = last;
}

void Timer::Reset() {
  last = svcGetSystemTick();
  current = last;
}

void Timer::Tick() {
  if (is_running) current = svcGetSystemTick();
}

void Timer::Pause() { is_running = false; }

void Timer::Resume() { is_running = true; }

bool Timer::Running() { return is_running; }

float Timer::Get() { return (float)((current - last) / TPMS); }

float Timer::GetLive() { return (float)((svcGetSystemTick() - last) / TPMS); }
}  // namespace RenderD7