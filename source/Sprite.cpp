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

#include <renderd7/Sprite.hpp>

void RenderD7::Sprite::FromSheet(RenderD7::Sheet *sheet, size_t index) {
  C2D_SpriteFromSheet(&this->sprite, sheet->spritesheet, index);
}
bool RenderD7::Sprite::Draw() {
  // Patch Depth before draw
  sprite.params.depth = 0.5;
  return C2D_DrawSprite(&this->sprite);
}
void RenderD7::Sprite::SetCenter(float x, float y) {
  C2D_SpriteSetCenter(&this->sprite, x, y);
}
void RenderD7::Sprite::SetPos(float x, float y) {
  C2D_SpriteSetPos(&this->sprite, x, y);
}
void RenderD7::Sprite::SetRotation(float rotation) {
  C2D_SpriteSetRotation(&this->sprite, rotation);
}
void RenderD7::Sprite::Rotate(float speed) {
  C2D_SpriteRotateDegrees(&this->sprite, speed);
}
float RenderD7::Sprite::getHeight() { return this->sprite.params.pos.h; }
float RenderD7::Sprite::getWidth() { return this->sprite.params.pos.w; }
float RenderD7::Sprite::getPosX() { return this->sprite.params.pos.x; }
float RenderD7::Sprite::getPosY() { return this->sprite.params.pos.y; }

void RenderD7::Sprite::FromImage(RenderD7::Image *img) {
  C2D_SpriteFromImage(&this->sprite, img->get());
}

void RenderD7::Sprite::SetScale(float x, float y) {
  C2D_SpriteScale(&this->sprite, x, y);
}