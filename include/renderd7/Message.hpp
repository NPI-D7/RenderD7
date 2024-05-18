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

#include <string>

namespace RenderD7 {
struct Message {
  Message(std::string t, std::string m) {
    title = t;
    message = m;
    animationframe = 0;
  }

  std::string title;
  std::string message;
  int animationframe;
};

void ProcessMessages();
void PushMessage(const Message& msg);
inline void PushMessage(const std::string& head, const std::string& msg) {
  PushMessage(Message(head, msg));
}
// Config
void SetMessageIdleStartFrame(int frame);
void SetMessageTotalAnimationFrames(int total_frames);
void SetMessageFadeOutStartFrame(int frame);
}  // namespace RenderD7