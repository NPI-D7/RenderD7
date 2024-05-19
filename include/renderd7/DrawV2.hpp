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

#include <citro2d.h>

#include <renderd7/Color.hpp>
#include <renderd7/Image.hpp>
#include <renderd7/R7Vec.hpp>
#include <renderd7/font.hpp>

#define MAKEFLAG(x) (1 << x)

typedef unsigned int RD7TextFlags;

enum RD7TextFlags_ {
  RD7TextFlags_None = 0,  //< Align is Left and Other things are disabled
  RD7TextFlags_AlignRight = MAKEFLAG(0),
  RD7TextFlags_AlignMid = MAKEFLAG(1),
  RD7TextFlags_Shaddow = MAKEFLAG(2),  // TextBuf Killer lol (doubled Text)
  RD7TextFlags_Wrap = MAKEFLAG(3),
  RD7TextFlags_Short = MAKEFLAG(4),
  RD7TextFlags_Scroll = MAKEFLAG(5),
};

namespace RenderD7 {
R7Vec2 GetTextDimensions(const std::string& text);
void CustomTextSize(float size);
void TextDefaultSize();
float TextGetSize();
std::string TextShort(const std::string& in, int max_len);
// Overrite TextBox Size (by default Screenwidth x Text.h)
void TextMaxBox(R7Vec2 size);
void TextDefaultBox();
void TextFont(Font::Ref fnt);
void TextFontRestore();
void TextDefaultFont();
namespace Draw2 {
void Rect(R7Vec2 pos, R7Vec2 size, unsigned int color, int t = 1);
void RectFilled(R7Vec2 pos, R7Vec2 size, Color4 colors);
void RectFilledSolid(R7Vec2 pos, R7Vec2 size, unsigned int color);
// Wrapper of RectFilledSolid
inline void RFS(R7Vec2 pos, R7Vec2 size, unsigned int color) {
  RectFilledSolid(pos, size, color);
}
void Line(R7Vec2 pos0, R7Vec2 pos1, unsigned int color, int t = 1);
void Triangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, Color3 colors);
void TriangleSolid(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int color);
// Beta and Very unstable
void TriangleLined(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int color,
                   int t = 1);
void Text(R7Vec2 pos, const std::string& text, RD7TextFlags flags = 0);
void Image(RenderD7::Image* img, const R7Vec2& pos = R7Vec2(0, 0),
           const R7Vec2& scale = R7Vec2(1, 1));
}  // namespace Draw2
}  // namespace RenderD7
