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

namespace RenderD7 {
namespace Hardware {
/// @brief Initialisize required Services
void Initialisize();
/// @brief Check if Headphones are Plugged in
/// @return true if headphones plugged in
bool IsHeadphones();
/// @brief Check if the 3ds Is Charging
/// @return true if System gets Charged
bool IsCharging();
/// @brief Check the Battery Percentage
/// @return Persentage as float
float GetBatteryPercentage();
/// @brief Get current State of 3d Slider
/// @return current 3dslider poition
float Get3dSliderLevel();
/// @brief Get Current state of Sound Slider
/// @return current SoundSlider state
float GetSoundSliderLevel();
}  // namespace Hardware
}  // namespace RenderD7