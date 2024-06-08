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

#include <map>
#include <renderd7/Color.hpp>
#include <renderd7/Font.hpp>
#include <renderd7/Image.hpp>
#include <renderd7/R7Vec.hpp>
#include <renderd7/Sprite.hpp>
#include <renderd7/smart_ctor.hpp>

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

enum R2Screen {
  R2Screen_Bottom,
  R2Screen_Top,
  // TopRight,
};

namespace RenderD7 {
class R2Base {
 public:
  struct R2Cmd {
    R7Vec2 pos;        //< Position
    R7Vec2 pszs;       //< Position or (TextBox) Size
    R7Vec2 ap;         //< Additional Pos
    unsigned int clr;  //< Color
    bool Screen;       //< TopScreen
    Image::Ref img;    //< Image Reference
    Sprite::Ref spr;   //< Sprite Reference
    // 0 = skip, 1 = rect, 2 = tri, 3 = text,
    // 4 = image, 5 = sprite
    int type;            //< Command Type
    bool lined = false;  //< Draw Lined Rect/Tri
    // Text Specific
    RD7TextFlags flags;  // Text Flags
    std::string text;    // Text
  };
  R2Base();
  ~R2Base() = default;
  RD7_SMART_CTOR(R2Base)

  // Settings
  void SetFont(Font::Ref fnt);
  Font::Ref GetFont();
  void DefaultFont();
  void DrawNextLined();
  void OnScreen(R2Screen screen);
  R2Screen GetCurrentScreen();
  void SetTextSize(float szs);
  void DefaultTextSize();
  float GetTextSize();

  // Processing
  void Process();
  R7Vec2 GetTextDimensions(const std::string& text);

  // Draw Functions
  void AddRect(R7Vec2 pos, R7Vec2 size, RD7Color clr);
  void AddRect(R7Vec2 pos, R7Vec2 size, unsigned int clr);
  void AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, RD7Color clr);
  void AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int clr);
  void AddText(R7Vec2 pos, const std::string& text, RD7Color clr,
               RD7TextFlags flags = 0, R7Vec2 tmb = R7Vec2());
  void AddText(R7Vec2 pos, const std::string& text, unsigned int clr,
               RD7TextFlags flags = 0, R7Vec2 tmb = R7Vec2());
  void AddImage(R7Vec2 pos, Image::Ref img);
  void AddSprite(Sprite::Ref spr);

 private:
  const float default_text_size = 0.5f;
  float text_size = 0.5;
  Font::Ref font;
  std::map<std::string, float> ts;
  std::map<std::string, int> mln;
  bool next_lined = false;
  std::vector<R2Cmd> commands;
  R2Screen current_screen = R2Screen_Bottom;
};
}  // namespace RenderD7