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

#include <renderd7/Hardware.hpp>
#include <renderd7/Security.hpp>

// Os Specific includes
#include <3ds.h>

// RenderD7 Security
extern bool isndspinit;

void RenderD7::Hardware::Initialisize() {
  rd7_security->SafeInit(mcuHwcInit, &mcuHwcExit);
  rd7_security->SafeInit(ptmuInit, &ptmuExit);
}

bool RenderD7::Hardware::IsHeadphones() {
  if (isndspinit) {
    bool inserted;
    DSP_GetHeadphoneStatus(&inserted);
    return inserted;
  } else
    return false;
}

bool RenderD7::Hardware::IsCharging() {
  uint8_t var;
  PTMU_GetBatteryChargeState(&var);
  // Some Security Stuff
  if (var < 0x00 && var > 0x01) {
    return false;
  }
  return (var == 0x01 ? true : false);
}

float RenderD7::Hardware::GetBatteryPercentage() {
  uint8_t percentLevel;
  PTMU_GetBatteryLevel(&percentLevel);
  return (float)percentLevel / 100;
}

float RenderD7::Hardware::Get3dSliderLevel() { return osGet3DSliderState(); }

float RenderD7::Hardware::GetSoundSliderLevel() {
  uint8_t percentLevel;
  MCUHWC_GetSoundSliderLevel(&percentLevel);
  return (float)percentLevel / 100;
}