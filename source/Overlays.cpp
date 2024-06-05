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

#include <renderd7/DrawV2.hpp>
#include <renderd7/FunctionTrace.hpp>
#include <renderd7/Hid.hpp>
#include <renderd7/Overlays.hpp>
#include <renderd7/internal_db.hpp>
#include <renderd7/renderd7.hpp>

///////////////////////////////
struct Key {
  std::string disp;
  R7Vec2 pos;
  R7Vec2 size;
  // 0 = default key
  // 1 = Shift
  // 2 = Backsp
  // 3 = Enter
  // 4 = Cancel
  // 5 = Confirm
  // 6 = Tab
  // 7 = Caps
  // 8 = Space
  int action = 0;
};

std::vector<Key> keyboard_layout_num{
    // 1st row
    {"7", R7Vec2(5, 135), R7Vec2(36, 24), 0},
    {"8", R7Vec2(43, 135), R7Vec2(36, 24), 0},
    {"9", R7Vec2(81, 135), R7Vec2(36, 24), 0},
    // 2nd row
    {"4", R7Vec2(5, 161), R7Vec2(36, 24), 0},
    {"5", R7Vec2(43, 161), R7Vec2(36, 24), 0},
    {"6", R7Vec2(81, 161), R7Vec2(36, 24), 0},
    // 3rd row
    {"1", R7Vec2(5, 187), R7Vec2(36, 24), 0},
    {"2", R7Vec2(43, 187), R7Vec2(36, 24), 0},
    {"3", R7Vec2(81, 187), R7Vec2(36, 24), 0},

    // 4th row
    {"0", R7Vec2(5, 213), R7Vec2(74, 24), 0},
    {".", R7Vec2(81, 213), R7Vec2(36, 24), 0},
    // additional actions
    {"bksp", R7Vec2(119, 135), R7Vec2(74, 24), 2},
    //{"", R7Vec2(119, 161), R7Vec2(74, 24), 0},
    {"Confirm", R7Vec2(119, 187), R7Vec2(74, 24), 5},
    {"Cancel", R7Vec2(119, 213), R7Vec2(74, 24), 4},
};

std::vector<Key> keyboard_layout = {
    // 1st row
    {"`", R7Vec2(5, 137), R7Vec2(18, 18), 0},
    {"1", R7Vec2(25, 137), R7Vec2(18, 18), 0},
    {"2", R7Vec2(45, 137), R7Vec2(18, 18), 0},
    {"3", R7Vec2(65, 137), R7Vec2(18, 18), 0},
    {"4", R7Vec2(85, 137), R7Vec2(18, 18), 0},
    {"5", R7Vec2(105, 137), R7Vec2(18, 18), 0},
    {"6", R7Vec2(125, 137), R7Vec2(18, 18), 0},
    {"7", R7Vec2(145, 137), R7Vec2(18, 18), 0},
    {"8", R7Vec2(165, 137), R7Vec2(18, 18), 0},
    {"9", R7Vec2(185, 137), R7Vec2(18, 18), 0},
    {"0", R7Vec2(205, 137), R7Vec2(18, 18), 0},
    {"-", R7Vec2(225, 137), R7Vec2(18, 18), 0},
    {"=", R7Vec2(245, 137), R7Vec2(18, 18), 0},
    {"Bksp", R7Vec2(265, 137), R7Vec2(50, 18), 2},
    // 2nd row
    {"Tab", R7Vec2(5, 157), R7Vec2(40, 18), 6},
    {"q", R7Vec2(47, 157), R7Vec2(18, 18), 0},
    {"w", R7Vec2(67, 157), R7Vec2(18, 18), 0},
    {"e", R7Vec2(87, 157), R7Vec2(18, 18), 0},
    {"r", R7Vec2(107, 157), R7Vec2(18, 18), 0},
    {"t", R7Vec2(127, 157), R7Vec2(18, 18), 0},
    {"y", R7Vec2(147, 157), R7Vec2(18, 18), 0},
    {"u", R7Vec2(167, 157), R7Vec2(18, 18), 0},
    {"i", R7Vec2(187, 157), R7Vec2(18, 18), 0},
    {"o", R7Vec2(207, 157), R7Vec2(18, 18), 0},
    {"p", R7Vec2(227, 157), R7Vec2(18, 18), 0},
    {"[", R7Vec2(247, 157), R7Vec2(18, 18), 0},
    {"]", R7Vec2(267, 157), R7Vec2(18, 18), 0},
    {"\\", R7Vec2(287, 157), R7Vec2(28, 18), 0},
    // 3rd row
    {"Caps", R7Vec2(5, 177), R7Vec2(50, 18), 7},
    {"a", R7Vec2(57, 177), R7Vec2(18, 18), 0},
    {"s", R7Vec2(77, 177), R7Vec2(18, 18), 0},
    {"d", R7Vec2(97, 177), R7Vec2(18, 18), 0},
    {"f", R7Vec2(117, 177), R7Vec2(18, 18), 0},
    {"g", R7Vec2(137, 177), R7Vec2(18, 18), 0},
    {"h", R7Vec2(157, 177), R7Vec2(18, 18), 0},
    {"j", R7Vec2(177, 177), R7Vec2(18, 18), 0},
    {"k", R7Vec2(197, 177), R7Vec2(18, 18), 0},
    {"l", R7Vec2(217, 177), R7Vec2(18, 18), 0},
    {";", R7Vec2(237, 177), R7Vec2(18, 18), 0},
    {"'", R7Vec2(257, 177), R7Vec2(18, 18), 0},
    {"Enter", R7Vec2(277, 177), R7Vec2(38, 18), 3},
    // 4th row
    {"Shift", R7Vec2(5, 197), R7Vec2(60, 18), 1},
    {"z", R7Vec2(67, 197), R7Vec2(18, 18), 0},
    {"x", R7Vec2(87, 197), R7Vec2(18, 18), 0},
    {"c", R7Vec2(107, 197), R7Vec2(18, 18), 0},
    {"v", R7Vec2(127, 197), R7Vec2(18, 18), 0},
    {"b", R7Vec2(147, 197), R7Vec2(18, 18), 0},
    {"n", R7Vec2(167, 197), R7Vec2(18, 18), 0},
    {"m", R7Vec2(187, 197), R7Vec2(18, 18), 0},
    {",", R7Vec2(207, 197), R7Vec2(18, 18), 0},
    {".", R7Vec2(227, 197), R7Vec2(18, 18), 0},
    {"/", R7Vec2(247, 197), R7Vec2(18, 18), 0},
    {"Shift", R7Vec2(267, 197), R7Vec2(48, 18), 1},
    // 5th row
    {"Cancel", R7Vec2(5, 217), R7Vec2(70, 18), 4},
    {"(X)", R7Vec2(77, 217), R7Vec2(23, 18), 10},
    {"Space", R7Vec2(102, 217), R7Vec2(108, 18), 8},
    {"(!)", R7Vec2(212, 217), R7Vec2(23, 18), 10},
    {"Confirm", R7Vec2(237, 217), R7Vec2(78, 18), 5},
    /*{"←", R7Vec2(237, 217), R7Vec2(18, 18)},
    {"→", R7Vec2(257, 217), R7Vec2(18, 18)},
    {"↓", R7Vec2(277, 217), R7Vec2(18, 18)},
    {"↑", R7Vec2(297, 217), R7Vec2(18, 18)},*/
};

std::vector<Key> keyboard_layout_caps = {
    // 1st row
    {"`", R7Vec2(5, 137), R7Vec2(18, 18), 0},
    {"1", R7Vec2(25, 137), R7Vec2(18, 18), 0},
    {"2", R7Vec2(45, 137), R7Vec2(18, 18), 0},
    {"3", R7Vec2(65, 137), R7Vec2(18, 18), 0},
    {"4", R7Vec2(85, 137), R7Vec2(18, 18), 0},
    {"5", R7Vec2(105, 137), R7Vec2(18, 18), 0},
    {"6", R7Vec2(125, 137), R7Vec2(18, 18), 0},
    {"7", R7Vec2(145, 137), R7Vec2(18, 18), 0},
    {"8", R7Vec2(165, 137), R7Vec2(18, 18), 0},
    {"9", R7Vec2(185, 137), R7Vec2(18, 18), 0},
    {"0", R7Vec2(205, 137), R7Vec2(18, 18), 0},
    {"-", R7Vec2(225, 137), R7Vec2(18, 18), 0},
    {"=", R7Vec2(245, 137), R7Vec2(18, 18), 0},
    {"Bksp", R7Vec2(265, 137), R7Vec2(50, 18), 2},
    // 2nd row
    {"Tab", R7Vec2(5, 157), R7Vec2(40, 18), 6},
    {"Q", R7Vec2(47, 157), R7Vec2(18, 18), 0},
    {"W", R7Vec2(67, 157), R7Vec2(18, 18), 0},
    {"E", R7Vec2(87, 157), R7Vec2(18, 18), 0},
    {"R", R7Vec2(107, 157), R7Vec2(18, 18), 0},
    {"T", R7Vec2(127, 157), R7Vec2(18, 18), 0},
    {"Y", R7Vec2(147, 157), R7Vec2(18, 18), 0},
    {"U", R7Vec2(167, 157), R7Vec2(18, 18), 0},
    {"I", R7Vec2(187, 157), R7Vec2(18, 18), 0},
    {"O", R7Vec2(207, 157), R7Vec2(18, 18), 0},
    {"P", R7Vec2(227, 157), R7Vec2(18, 18), 0},
    {"[", R7Vec2(247, 157), R7Vec2(18, 18), 0},
    {"]", R7Vec2(267, 157), R7Vec2(18, 18), 0},
    {"\\", R7Vec2(287, 157), R7Vec2(28, 18), 0},
    // 3rd row
    {"Caps", R7Vec2(5, 177), R7Vec2(50, 18), 7},
    {"A", R7Vec2(57, 177), R7Vec2(18, 18), 0},
    {"S", R7Vec2(77, 177), R7Vec2(18, 18), 0},
    {"D", R7Vec2(97, 177), R7Vec2(18, 18), 0},
    {"F", R7Vec2(117, 177), R7Vec2(18, 18), 0},
    {"G", R7Vec2(137, 177), R7Vec2(18, 18), 0},
    {"H", R7Vec2(157, 177), R7Vec2(18, 18), 0},
    {"J", R7Vec2(177, 177), R7Vec2(18, 18), 0},
    {"K", R7Vec2(197, 177), R7Vec2(18, 18), 0},
    {"L", R7Vec2(217, 177), R7Vec2(18, 18), 0},
    {";", R7Vec2(237, 177), R7Vec2(18, 18), 0},
    {"'", R7Vec2(257, 177), R7Vec2(18, 18), 0},
    {"Enter", R7Vec2(277, 177), R7Vec2(38, 18), 3},
    // 4th row
    {"Shift", R7Vec2(5, 197), R7Vec2(60, 18), 1},
    {"Z", R7Vec2(67, 197), R7Vec2(18, 18), 0},
    {"X", R7Vec2(87, 197), R7Vec2(18, 18), 0},
    {"C", R7Vec2(107, 197), R7Vec2(18, 18), 0},
    {"V", R7Vec2(127, 197), R7Vec2(18, 18), 0},
    {"B", R7Vec2(147, 197), R7Vec2(18, 18), 0},
    {"N", R7Vec2(167, 197), R7Vec2(18, 18), 0},
    {"M", R7Vec2(187, 197), R7Vec2(18, 18), 0},
    {",", R7Vec2(207, 197), R7Vec2(18, 18), 0},
    {".", R7Vec2(227, 197), R7Vec2(18, 18), 0},
    {"/", R7Vec2(247, 197), R7Vec2(18, 18), 0},
    {"Shift", R7Vec2(267, 197), R7Vec2(48, 18), 1},
    // 5th row
    {"Cancel", R7Vec2(5, 217), R7Vec2(70, 18), 4},
    {"(X)", R7Vec2(77, 217), R7Vec2(23, 18), 10},
    {"Space", R7Vec2(102, 217), R7Vec2(108, 18), 8},
    {"(!)", R7Vec2(212, 217), R7Vec2(23, 18), 10},
    {"Confirm", R7Vec2(237, 217), R7Vec2(78, 18), 5},
    /*{"←", R7Vec2(237, 217), R7Vec2(18, 18)},
    {"→", R7Vec2(257, 217), R7Vec2(18, 18)},
    {"↑", R7Vec2(277, 217), R7Vec2(18, 18)},
    {"↓", R7Vec2(297, 217), R7Vec2(18, 18)},*/
};

std::vector<Key> keyboard_layout_shift = {
    // 1st row
    {"~", R7Vec2(5, 137), R7Vec2(18, 18), 0},
    {"!", R7Vec2(25, 137), R7Vec2(18, 18), 0},
    {"@", R7Vec2(45, 137), R7Vec2(18, 18), 0},
    {"#", R7Vec2(65, 137), R7Vec2(18, 18), 0},
    {"$", R7Vec2(85, 137), R7Vec2(18, 18), 0},
    {"%", R7Vec2(105, 137), R7Vec2(18, 18), 0},
    {"^", R7Vec2(125, 137), R7Vec2(18, 18), 0},
    {"&", R7Vec2(145, 137), R7Vec2(18, 18), 0},
    {"*", R7Vec2(165, 137), R7Vec2(18, 18), 0},
    {"(", R7Vec2(185, 137), R7Vec2(18, 18), 0},
    {")", R7Vec2(205, 137), R7Vec2(18, 18), 0},
    {"_", R7Vec2(225, 137), R7Vec2(18, 18), 0},
    {"+", R7Vec2(245, 137), R7Vec2(18, 18), 0},
    {"Bksp", R7Vec2(265, 137), R7Vec2(50, 18), 2},
    // 2nd row
    {"Tab", R7Vec2(5, 157), R7Vec2(40, 18), 6},
    {"Q", R7Vec2(47, 157), R7Vec2(18, 18), 0},
    {"W", R7Vec2(67, 157), R7Vec2(18, 18), 0},
    {"E", R7Vec2(87, 157), R7Vec2(18, 18), 0},
    {"R", R7Vec2(107, 157), R7Vec2(18, 18), 0},
    {"T", R7Vec2(127, 157), R7Vec2(18, 18), 0},
    {"Y", R7Vec2(147, 157), R7Vec2(18, 18), 0},
    {"U", R7Vec2(167, 157), R7Vec2(18, 18), 0},
    {"I", R7Vec2(187, 157), R7Vec2(18, 18), 0},
    {"O", R7Vec2(207, 157), R7Vec2(18, 18), 0},
    {"P", R7Vec2(227, 157), R7Vec2(18, 18), 0},
    {"{", R7Vec2(247, 157), R7Vec2(18, 18), 0},
    {"}", R7Vec2(267, 157), R7Vec2(18, 18), 0},
    {"|", R7Vec2(287, 157), R7Vec2(28, 18), 0},
    // 3rd row
    {"Caps", R7Vec2(5, 177), R7Vec2(50, 18), 7},
    {"A", R7Vec2(57, 177), R7Vec2(18, 18), 0},
    {"S", R7Vec2(77, 177), R7Vec2(18, 18), 0},
    {"D", R7Vec2(97, 177), R7Vec2(18, 18), 0},
    {"F", R7Vec2(117, 177), R7Vec2(18, 18), 0},
    {"G", R7Vec2(137, 177), R7Vec2(18, 18), 0},
    {"H", R7Vec2(157, 177), R7Vec2(18, 18), 0},
    {"J", R7Vec2(177, 177), R7Vec2(18, 18), 0},
    {"K", R7Vec2(197, 177), R7Vec2(18, 18), 0},
    {"L", R7Vec2(217, 177), R7Vec2(18, 18), 0},
    {":", R7Vec2(237, 177), R7Vec2(18, 18), 0},
    {"\"", R7Vec2(257, 177), R7Vec2(18, 18), 0},
    {"Enter", R7Vec2(277, 177), R7Vec2(38, 18), 3},
    // 4th row
    {"Shift", R7Vec2(5, 197), R7Vec2(60, 18), 1},
    {"Z", R7Vec2(67, 197), R7Vec2(18, 18), 0},
    {"X", R7Vec2(87, 197), R7Vec2(18, 18), 0},
    {"C", R7Vec2(107, 197), R7Vec2(18, 18), 0},
    {"V", R7Vec2(127, 197), R7Vec2(18, 18), 0},
    {"B", R7Vec2(147, 197), R7Vec2(18, 18), 0},
    {"N", R7Vec2(167, 197), R7Vec2(18, 18), 0},
    {"M", R7Vec2(187, 197), R7Vec2(18, 18), 0},
    {"<", R7Vec2(207, 197), R7Vec2(18, 18), 0},
    {">", R7Vec2(227, 197), R7Vec2(18, 18), 0},
    {"?", R7Vec2(247, 197), R7Vec2(18, 18), 0},
    {"Shift", R7Vec2(267, 197), R7Vec2(48, 18), 1},
    // 5th row
    {"Cancel", R7Vec2(5, 217), R7Vec2(70, 18), 4},
    {"(X)", R7Vec2(77, 217), R7Vec2(23, 18), 10},
    {"Space", R7Vec2(102, 217), R7Vec2(108, 18), 8},
    {"(!)", R7Vec2(212, 217), R7Vec2(23, 18), 10},
    {"Confirm", R7Vec2(237, 217), R7Vec2(78, 18), 5},
    /*{"←", R7Vec2(237, 217), R7Vec2(18, 18)},
    {"→", R7Vec2(257, 217), R7Vec2(18, 18)},
    {"↑", R7Vec2(277, 217), R7Vec2(18, 18)},
    {"↓", R7Vec2(297, 217), R7Vec2(18, 18)},*/
};

// From UI7
bool UI7_InBox(R7Vec2 inpos, R7Vec2 boxpos, R7Vec2 boxsize) {
  if ((inpos.x > boxpos.x) && (inpos.y > boxpos.y) &&
      (inpos.x < boxpos.x + boxsize.x) && (inpos.y < boxpos.y + boxsize.y))
    return true;
  return false;
}

namespace RenderD7 {
Ovl_Ftrace::Ovl_Ftrace(bool* is_enabled) { i_is_enabled = is_enabled; }

void Ovl_Ftrace::Draw(void) const {
  float tmp_txt = RenderD7::TextGetSize();
  RenderD7::TextDefaultSize();
  RenderD7::OnScreen(Top);
  RenderD7::Color::RGBA bg(RD7Color_Background);
  bg.changeA(150);
  RenderD7::Draw2::RFS(R7Vec2(0, 0), R7Vec2(400, 20), bg.toRGBA());

  std::vector<RenderD7::Ftrace::FTRes> dt;
  for (auto const& it : RenderD7::Ftrace::rd7_traces)
    if (it.second.is_ovl && dt.size() < 10) dt.push_back(it.second);
  for (size_t i = 0; i < (dt.size() < 10 ? dt.size() : 10); i++) {
    RenderD7::Draw2::Text(R7Vec2(5, 30 + i * 15), dt[i].func_name);
    RenderD7::Draw2::Text(R7Vec2(295, 30 + i * 15),
                          RenderD7::MsTimeFmt(dt[i].time_of));
  }
  RenderD7::CustomTextSize(tmp_txt);
}

void Ovl_Ftrace::Logic() {
  if (!i_is_enabled[0]) this->Kill();
}

Ovl_Metrik::Ovl_Metrik(bool* is_enabled, bool* screen, uint32_t* mt_color,
                       uint32_t* txt_color, float* txt_size) {
  i_is_enabled = is_enabled;
  i_screen = screen;
  i_mt_color = mt_color;
  i_txt_color = txt_color;
  i_txt_size = txt_size;
}

void Ovl_Metrik::Draw(void) const {
  float tmp_txt = RenderD7::TextGetSize();
  RenderD7::TextDefaultSize();
  if (i_screen[0]) {
    RenderD7::OnScreen(Bottom);
  } else {
    RenderD7::OnScreen(Top);
  }
  std::string info =
      "RenderD7 " + std::string(RENDERD7VSTRING) + " Debug Overlay";
  float dim_y = RenderD7::GetTextDimensions(info).y;
  float infoy = 240 - dim_y;
  mt_fps = "FPS: " + RenderD7::GetFramerate();
  if (rd7i_idb_running) mt_fps += " IDB -> ON";
  mt_cpu = "CPU: " +
           std::to_string(C3D_GetProcessingTime() * (RenderD7::GetFps() / 10))
               .substr(0, 4) +
           "%/" + std::to_string(C3D_GetProcessingTime()).substr(0, 4) + "ms";
  mt_gpu = "GPU: " +
           std::to_string(C3D_GetDrawingTime() * (RenderD7::GetFps() / 10))
               .substr(0, 4) +
           "%/" + std::to_string(C3D_GetDrawingTime()).substr(0, 4) + "ms";
  mt_cmd =
      "CMD: " + std::to_string(C3D_GetCmdBufUsage() * 100.0f).substr(0, 4) +
      "%";
  mt_lfr = "Linear: " + RenderD7::FormatBytes(linearSpaceFree());
  mt_tbs =
      "TextBuf: " + std::to_string(C2D_TextBufGetNumGlyphs(rd7i_text_buffer)) +
      "/4096";
  RenderD7::Draw2::RFS(R7Vec2(0, 0), RenderD7::GetTextDimensions(mt_fps),
                       i_mt_color[0]);
  RenderD7::Draw2::RFS(R7Vec2(0, 50), RenderD7::GetTextDimensions(mt_cpu),
                       i_mt_color[0]);
  RenderD7::Draw2::RFS(R7Vec2(0, 50 + dim_y * 1),
                       RenderD7::GetTextDimensions(mt_gpu), i_mt_color[0]);
  RenderD7::Draw2::RFS(R7Vec2(0, 50 + dim_y * 2),
                       RenderD7::GetTextDimensions(mt_cmd), i_mt_color[0]);
  RenderD7::Draw2::RFS(R7Vec2(0, 50 + dim_y * 3),
                       RenderD7::GetTextDimensions(mt_lfr), i_mt_color[0]);
  RenderD7::Draw2::RFS(R7Vec2(0, 50 + dim_y * 4),
                       RenderD7::GetTextDimensions(mt_tbs), i_mt_color[0]);
  RenderD7::Draw2::RFS(R7Vec2(0, infoy), RenderD7::GetTextDimensions(info),
                       i_mt_color[0]);
  RenderD7::ThemeActive()->Set(RD7Color_Text, i_txt_color[0]);
  RenderD7::Draw2::Text(R7Vec2(0, 0), mt_fps);
  RenderD7::Draw2::Text(R7Vec2(0, 50), mt_cpu);
  RenderD7::Draw2::Text(R7Vec2(0, 50 + dim_y * 1), mt_gpu);
  RenderD7::Draw2::Text(R7Vec2(0, 50 + dim_y * 2), mt_cmd);
  RenderD7::Draw2::Text(R7Vec2(0, 50 + dim_y * 3), mt_lfr);
  RenderD7::Draw2::Text(R7Vec2(0, 50 + dim_y * 4), mt_tbs);
  RenderD7::Draw2::Text(R7Vec2(0, infoy), info);
  RenderD7::ThemeActive()->Undo();

  // Force Bottom (Debug Touchpos)
  RenderD7::OnScreen(Bottom);
  if (Hid::IsEvent("touch", Hid::Held)) {
    RenderD7::Draw2::Line(R7Vec2(Hid::GetTouchPosition().x, 0),
                          R7Vec2(Hid::GetTouchPosition().x, 240),
                          RenderD7::Color::Hex("#ff0000"));
    RenderD7::Draw2::Line(R7Vec2(0, Hid::GetTouchPosition().y),
                          R7Vec2(320, Hid::GetTouchPosition().y),
                          RenderD7::Color::Hex("#ff0000"));
  }
  RenderD7::CustomTextSize(tmp_txt);
}

void Ovl_Metrik::Logic() {
  if (!i_is_enabled[0]) this->Kill();
}

Ovl_Keyboard::Ovl_Keyboard(std::string& ref, RD7KeyboardState& state,
                           const std::string& hint, RD7Keyboard type) {
  // Blocks All Input outside of Keyboard
  // Doesnt work for Hidkeys down etc
  RenderD7::Hid::Lock();
  typed_text = &ref;
  this->state = &state;
  this->type = type;
  *this->state = RD7KeyboardState_None;
  str_bak = ref;
  ft3 = 0;
}

Ovl_Keyboard::~Ovl_Keyboard() {
  // And Unlock when closing Keyboard lol
  RenderD7::Hid::Unlock();
}

void Ovl_Keyboard::Draw(void) const {
  float tmp_txt = RenderD7::TextGetSize();
  RenderD7::TextDefaultSize();
  if (ft3 > 5) RenderD7::Hid::Unlock();
  auto key_table =
      (type == RD7Keyboard_Numpad) ? keyboard_layout_num : keyboard_layout;
  if (mode == 1)
    key_table = keyboard_layout_caps;
  else if (mode == 2)
    key_table = keyboard_layout_shift;
  RenderD7::OnScreen(Top);
  RenderD7::Draw2::RFS(
      R7Vec2(0, 0), R7Vec2(400, 240),
      RenderD7::Color::RGBA(RD7Color_FrameBg).changeA(150).toRGBA());
  RenderD7::OnScreen(Bottom);
  RenderD7::Draw2::RFS(
      R7Vec2(0, 0), R7Vec2(320, 112),
      RenderD7::Color::RGBA(RD7Color_FrameBg).changeA(150).toRGBA());
  RenderD7::Draw2::RFS(R7Vec2(0, 112), R7Vec2(320, 128),
                       RenderD7::ThemeActive()->Get(RD7Color_FrameBg));
  RenderD7::Draw2::RFS(R7Vec2(0, 112), R7Vec2(320, 20),
                       RenderD7::ThemeActive()->Get(RD7Color_Header));
  RenderD7::ThemeActive()->TextBy(RD7Color_Header);
  RenderD7::Draw2::Text(R7Vec2(5, 114), "> " + *typed_text);
  RenderD7::ThemeActive()->Undo();
  for (auto const& it : key_table) {
    R7Vec2 txtdim = RenderD7::GetTextDimensions(it.disp);
    R7Vec2 txtpos = R7Vec2(it.pos.x + it.size.x * 0.5 - txtdim.x * 0.5,
                           it.pos.y + it.size.y * 0.5 - txtdim.y * 0.5);
    RD7Color btn = RD7Color_Button;
    if (RenderD7::Hid::IsEvent("cancel", RenderD7::Hid::Up)) {
      RenderD7::Hid::Clear();
      shared_data[0x05] = 1;
    }
    if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
        UI7_InBox(RenderD7::Hid::GetLastTouchPosition(), it.pos, it.size)) {
      if (mode == 2)  // Request Disable Shift
        shared_data[0x02] = 1;

      if (it.action == 0)
        shared_data[0x01] = it.disp[0];
      else if (it.action == 1)
        shared_data[0x02] = 1;
      else if (it.action == 2)
        shared_data[0x03] = 1;
      else if (it.action == 3)
        shared_data[0x04] = 1;
      else if (it.action == 4)
        shared_data[0x05] = 1;
      else if (it.action == 5)
        shared_data[0x06] = 1;
      else if (it.action == 6)
        shared_data[0x07] = 1;
      else if (it.action == 7)
        shared_data[0x08] = 1;
      else if (it.action == 8)
        shared_data[0x09] = 1;
    } else if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Held) &&
               UI7_InBox(RenderD7::Hid::GetTouchPosition(), it.pos, it.size)) {
      btn = RD7Color_ButtonHovered;
    }
    RenderD7::Draw2::RFS(it.pos, it.size, RenderD7::ThemeActive()->Get(btn));
    RenderD7::ThemeActive()->TextBy(btn);
    RenderD7::Draw2::Text(txtpos, it.disp);
    RenderD7::ThemeActive()->Undo();
  }
  if (ft3 > 5) RenderD7::Hid::Lock();
  RenderD7::CustomTextSize(tmp_txt);
}

void Ovl_Keyboard::Logic() {
  ft3++;
  for (const auto& it : shared_data) {
    if (it.first == 0x01) {
      typed_text->push_back(it.second);
    } else if (it.first == 0x02) {
      // Shift
      mode = (mode == 2) ? 0 : 2;
    } else if (it.first == 0x03) {
      if (typed_text->length() >= 1)
        typed_text->erase(typed_text->begin() + typed_text->length() - 1);
    } else if (it.first == 0x04) {
      // Enter
    } else if (it.first == 0x05) {
      *typed_text = str_bak;
      *state = RD7KeyboardState_Cancel;
      this->Kill();
    } else if (it.first == 0x06) {
      *state = RD7KeyboardState_Confirm;
      this->Kill();
    } else if (it.first == 0x07) {
      // this->typed_text += '\t';  // Tab
    } else if (it.first == 0x08) {
      // Caps
      mode = (mode == 1) ? 0 : 1;
    } else if (it.first == 0x09) {
      typed_text->append(" ");  // Space
    }
  }

  shared_data.clear();
}
}  // namespace RenderD7