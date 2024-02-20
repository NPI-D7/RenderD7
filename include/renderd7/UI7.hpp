#pragma once

#include <algorithm>
#include <renderd7/DrawV2.hpp>
#include <renderd7/R7Vec.hpp>
#include <string>
#include <vector>

// UI7: The new RenderD7 UI Standart based on
// Draw2 (based on Citro2D)

#define UI7MAKEFLAG(x) (1 << x)

typedef int UI7MenuFlags;

enum UI7MenuFlags_ {
  UI7MenuFlags_None = 0,
  UI7MenuFlags_NoTitlebar = UI7MAKEFLAG(0),
  UI7MenuFlags_TitleMid = UI7MAKEFLAG(1),
};

namespace UI7 {
// Key functions
void Init();
void Deinit();
void Update();
float GetTime();
float GetDeltaTime();

bool Button(const std::string &label, R7Vec2 size = R7Vec2(0, 0));
void Checkbox(const std::string &label, bool &c);
void Label(const std::string &label, RD7TextFlags flags = 0);
void Progressbar(float value);
/// @brief Draw Image in Menu
/// @param img Pointer f.e to RenderD7::Image2
void Image(RenderD7::Image *img);
void BrowserList(const std::vector<std::string> &entrys, int &selection,
                 RD7TextFlags txtflags = 0, R7Vec2 size = R7Vec2(0, 0),
                 int max_entrys = 13);
void InputText(const std::string &label, std::string &text,
               const std::string &hint = "");
bool BeginMenu(const std::string &title, R7Vec2 size = R7Vec2(0, 0),
               UI7MenuFlags flags = 0);
void EndMenu();
void Grid(const std::string &name, const R7Vec2 &size,
          R7Vec2 (*display_func)(void *, R7Vec2), void **data_array,
          size_t num_entrys);
bool BeginTree(const std::string &text);
void EndTree();
R7Vec2 GetCursorPos();
void SetCursorPos(R7Vec2 cp);
void RestoreCursor();
void SameLine();
}  // namespace UI7