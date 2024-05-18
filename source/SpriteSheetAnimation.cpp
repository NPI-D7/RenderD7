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

#include <renderd7/SpriteAnimation.hpp>
#include <renderd7/log.hpp>

extern Log renderd7log;

RenderD7::SpriteSheetAnimation::SpriteSheetAnimation() {
  //
}
RenderD7::SpriteSheetAnimation::~SpriteSheetAnimation() {
  //
}

void RenderD7::SpriteSheetAnimation::Setup(RenderD7::Sheet *sheet,
                                           size_t imagecount, size_t startimage,
                                           float frame_begin,
                                           float frame_finish) {
  D_totaltime = frame_begin;

  this->images = imagecount;

  this->sheet = sheet;

  this->time = frame_finish;

  RenderD7::SpriteSheetAnimation::FromSheet(this->sheet, startimage);
}
void RenderD7::SpriteSheetAnimation::Play(float timespeed) {
  D_totaltime += timespeed;
  if (D_totaltime >= time) {
    D_totaltime -= time;
    imgs++;
    if (imgs == images) {
      imgs = 0;
    }
  }
  RenderD7::SpriteSheetAnimation::FromSheet(sheet, imgs);
  // RenderD7::SpriteSheetAnimation::Draw();
}