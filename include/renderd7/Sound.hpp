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
#include <string>

namespace RenderD7 {
/** Sound Class */
class Sound {
 public:
  /// \brief Construct new Soundeffect
  /// \param path Path to the .wav file
  /// \param channel the channel 1-23
  /// \param toloop true:loop the sound, false: don't loop
  Sound(const std::string &path, int channel = 1, bool toloop = false);
  /// @brief Deconstructor
  ~Sound();
  RD7_SMART_CTOR(Sound)
  /// @brief Play the sound
  void Play();
  /// @brief Stop the sound
  void Stop();

 private:
  /// \param dataSize Size of the filedata
  u32 dataSize;
  /// \param waveBuf For ndsp
  ndspWaveBuf waveBuf;
  /// \param data Memmory data of the sound
  uint8_t *data = NULL;
  /// \param chnl Channel of the sound
  int chnl;
};
}  // namespace RenderD7