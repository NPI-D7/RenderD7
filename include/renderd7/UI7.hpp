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

#include <renderd7/Image.hpp>
#include <renderd7/R7Vec.hpp>
#include <renderd7/Render2.hpp>
#include <renderd7/smart_ctor.hpp>

// UI7: The new RenderD7 UI Standart based on
// Draw2 (based on Citro2D)

#define UI7MAKEFLAG(x) (1 << x)

typedef int UI7MenuFlags;

enum UI7MenuFlags_ {
  UI7MenuFlags_None = 0,
  UI7MenuFlags_NoTitlebar = UI7MAKEFLAG(0),
  UI7MenuFlags_TitleMid = UI7MAKEFLAG(1),
  UI7MenuFlags_Scrolling = MAKEFLAG(2),
};

class DrawCmd;
class UI7DrawList {
 public:
  UI7DrawList() = default;
  ~UI7DrawList() = default;

  void AddRectangle(R7Vec2 pos, R7Vec2 szs, RD7Color clr);
  void AddRectangle(R7Vec2 pos, R7Vec2 szs, unsigned int clr);
  void AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, RD7Color clr);
  void AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int clr);
  void AddText(R7Vec2 pos, const std::string &text, RD7Color clr,
               RD7TextFlags flags = 0, R7Vec2 box = R7Vec2());
  void AddText(R7Vec2 pos, const std::string &text, unsigned int clr,
               RD7TextFlags flags = 0, R7Vec2 box = R7Vec2());
  void AddImage(R7Vec2 pos, RenderD7::Image::Ref img);
  void AddCall(std::shared_ptr<DrawCmd> cmd);

  void Process(bool auto_clear = true);
  void Clear();

  RD7_SMART_CTOR(UI7DrawList)

 private:
  void AddDebugCall(std::shared_ptr<DrawCmd> cmd);
  std::vector<std::shared_ptr<DrawCmd>> list;
};

namespace UI7 {
// Key functions
void Init();
void Deinit();
void Update();
float GetTime();
float GetDeltaTime();
bool &IsDebugging();
// Internal Function
// Should not be used
void Debug();

bool Button(const std::string &label, R7Vec2 size = R7Vec2(0, 0));
void Checkbox(const std::string &label, bool &c);
void Label(const std::string &label, RD7TextFlags flags = 0);
void Progressbar(float value);
/// @brief Draw Image in Menu
/// @param img Pointer f.e to RenderD7::Image2
void Image(RenderD7::Image::Ref img);
void BrowserList(const std::vector<std::string> &entrys, int &selection,
                 RD7TextFlags txtflags = 0, R7Vec2 size = R7Vec2(0, 0),
                 int max_entrys = 13);
void InputText(const std::string &label, std::string &text,
               const std::string &hint = "");
bool BeginMenu(const std::string &title, R7Vec2 size = R7Vec2(0, 0),
               UI7MenuFlags flags = 0);
void EndMenu();
void Grid(const std::string &name, const R7Vec2 &size, const R7Vec2 &entry_size,
          void (*display_func)(void *, R7Vec2), void **data_array,
          size_t num_entrys);
void ColorSelector(const std::string &label, unsigned int &color);
bool BeginTree(const std::string &text);
void EndTree();
R7Vec2 GetCursorPos();
void SetCursorPos(R7Vec2 cp);
void RestoreCursor();
void SameLine();
float GetScrollingOffset();
namespace Menu {
  // All of them return the Main BG DrawList if Menu is null
  UI7DrawList::Ref GetBackgroundList();
  UI7DrawList::Ref GetList();
  UI7DrawList::Ref GetForegroundList();
}
// DrawLists
UI7DrawList::Ref GetForegroundList();
UI7DrawList::Ref GetBackgroundList();
}  // namespace UI7
