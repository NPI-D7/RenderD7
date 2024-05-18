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

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

#include <renderd7/Security.hpp>
#include <renderd7/internal_db.hpp>

RenderD7::Security *rd7_security;

bool *running_addr = NULL;
RenderD7::Security::Level rd7i_slvl = RenderD7::Security::Level::NONE;

RenderD7::Security::Level RenderD7::Security::GetLevel() { return rd7i_slvl; }

void RenderD7::Security::SetLevel(RenderD7::Security::Level level) {
  rd7i_slvl = level;
}

namespace RenderD7 {
Security::Security() { running_addr = &rd7i_running; }

Security::~Security() { *running_addr = false; }
void Security::SafeExit(void (*exit_func)()) { atexit(exit_func); }

void Security::SafeInit(void (*init_func)(), void (*exit_func)()) {
  init_func();
  atexit(exit_func);
}

void Security::SafeInit(Result (*init_func)(), void (*exit_func)()) {
  init_func();
  atexit(exit_func);
}

void Security::SafeInit(void (*init_func)(), Result (*exit_func)()) {
  init_func();
  atexit(reinterpret_cast<void (*)()>(exit_func));
}

void Security::SafeInit(Result (*init_func)(), Result (*exit_func)()) {
  init_func();
  atexit(reinterpret_cast<void (*)()>(exit_func));
}
}  // namespace RenderD7
namespace RenderD7 {
namespace Init {
void Security() {
  rd7_security = new RenderD7::Security();  // Load and Init the Security System
}
}  // namespace Init
}  // namespace RenderD7