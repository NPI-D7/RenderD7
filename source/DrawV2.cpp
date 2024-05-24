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

#include <citro2d.h>

#include <renderd7/DrawV2.hpp>
#include <renderd7/global_db.hpp>
#include <renderd7/internal_db.hpp>

const float rd7i_d7_dts = 0.5f;
float rd7i_d2_txt_size = rd7i_d7_dts;
C2D_Font rd7i_d2_fnt = nullptr;
C2D_Font rd7i_d2_fntb = nullptr;
R7Vec2 rd7i_d7_mwh = R7Vec2(0, 0);
std::map<std::string, float> rd7i_d2_ts;
std::map<std::string, int> rd7i_d2_mln;

bool rd7i_txt_init = false;

std::string GetShortedText(const std::string &in, int maxlen) {
  auto textdim = RenderD7::GetTextDimensions(in);
  if (textdim.x < (float)maxlen) return in;
  std::string ft = "";
  std::string worker = in;
  if (in.find_last_of('.') != in.npos) {
    ft = in.substr(in.find_last_of('.'));
    worker = in.substr(0, in.find_last_of('.'));
  }

  maxlen -= RenderD7::GetTextDimensions(ft).x -
            RenderD7::GetTextDimensions("(...)").x;
  float len_mod = (float)maxlen / textdim.x;
  int pos = (in.length() * len_mod) / rd7_draw2_tsm;
  std::string out;

  out = in.substr(0, pos);

  for (size_t i = pos; i < worker.length(); i++) {
    out += worker[i];
    if (RenderD7::GetTextDimensions(out + "(...)" + ft).x > (float)maxlen) {
      out += "(...)";
      out += ft;
      return out;
    }
  }
  return "";  // Impossible to reach
}

std::string WrapText(const std ::string &in, int maxlen) {
  std::string out;
  std::string line;
  int line_x = 0;
  std::istringstream istream(in);
  std::string temp;

  while (istream >> temp) {
    R7Vec2 dim = RenderD7::GetTextDimensions(line + temp);
    if (line_x + dim.x <= maxlen) {
      line += temp + ' ';
      line_x += dim.x;
    } else {
      out += line + '\n';
      line = temp + ' ';
      line_x = dim.x;
    }
  }
  out += line;
  return out;
}

bool RD7I_FNT_VALID() {
  if (rd7i_d2_fnt != nullptr && !rd7i_txt_init) {
    rd7i_txt_init = true;
  }
  if (!rd7i_txt_init) {
    if (rd7i_base_font != nullptr) {
      rd7i_d2_fnt = rd7i_base_font;
      rd7i_txt_init = true;
      return true;
    }
  }
  if (rd7i_d2_fnt != nullptr) {
    return true;
  } else {
    rd7i_d2_fnt = rd7i_base_font;
    return true;
  }
  // Schould never be reached
  return false;
}

namespace RenderD7 {
// TODO: Fix wrong Width/Height on other fonts
R7Vec2 GetTextDimensions(const std::string &text) {
  C2D_TextBufClear(rd7i_d2_dimbuf);
  float w = 0, h = 0;
  if (!RD7I_FNT_VALID()) return R7Vec2(w, h);
  C2D_Text c2dtext;
  C2D_TextFontParse(&c2dtext, rd7i_d2_fnt, rd7i_d2_dimbuf, text.c_str());
  C2D_TextGetDimensions(&c2dtext, rd7i_d2_txt_size, rd7i_d2_txt_size, &w, &h);
  return R7Vec2(w, h);
}

void CustomTextSize(float size) { rd7i_d2_txt_size = size; }

void TextDefaultSize() { rd7i_d2_txt_size = rd7i_d7_dts; }

void TextMaxBox(R7Vec2 size) { rd7i_d7_mwh = size; }

void TextDefaultBox() { rd7i_d7_mwh = R7Vec2(0, 0); }

void TextFont(Font::Ref fnt) {
  rd7i_d2_fntb = rd7i_d2_fnt;
  rd7i_d2_fnt = fnt->Ptr();
}

void TextFontRestore() {
  // Create temp copy
  // Restore other font
  // Set other to temp
  auto tmp = rd7i_d2_fnt;
  rd7i_d2_fnt = rd7i_d2_fntb;
  rd7i_d2_fntb = tmp;
}

void TextDefaultFont() {
  rd7i_d2_fntb = rd7i_d2_fnt;
  rd7i_d2_fnt = rd7i_base_font;
}

float TextGetSize() { return rd7i_d2_txt_size; }

std::string TextShort(const std::string &in, int max_len) {
  return GetShortedText(in, max_len);
}

namespace Draw2 {
void Scissor(R7Vec2 pos, R7Vec2 size) {
  // TODO: Seems not correct yet
  C3D_SetScissor(GPU_SCISSOR_NORMAL, GSP_SCREEN_WIDTH - pos.y - size.y, pos.x,
                 GSP_SCREEN_WIDTH - pos.y, pos.x + size.x);
}
void ScissorReset() { C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0); }
void Rect(R7Vec2 pos, R7Vec2 size, unsigned int color, int t) {
  // 4 DrawLine Calls Lol
  C2D_DrawLine(pos.x, pos.y, color, pos.x + size.x, pos.y, color, t, 1.f);
  C2D_DrawLine(pos.x, pos.y, color, pos.x, pos.y + size.y, color, t, 1.f);
  C2D_DrawLine(pos.x + size.x, pos.y, color, pos.x + size.x, pos.y + size.y,
               color, t, 1.f);
  C2D_DrawLine(pos.x, pos.y + size.y, color, pos.x + size.x, pos.y + size.y,
               color, t, 1.f);
}

void RectFilled(R7Vec2 pos, R7Vec2 size, Color4 colors) {
  C2D_DrawRectangle(pos.x, pos.y, 0.5f, size.x, size.y, colors.color0,
                    colors.color1, colors.color2, colors.color3);
}

void RectFilledSolid(R7Vec2 pos, R7Vec2 size, unsigned int color) {
  C2D_DrawRectSolid(pos.x, pos.y, 0.5f, size.x, size.y, color);
}

void Line(R7Vec2 pos0, R7Vec2 pos1, unsigned int color, int t) {
  C2D_DrawLine(pos0.x, pos0.y, color, pos1.x, pos1.y, color, t, 1.f);
}

void Triangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, Color3 colors) {
  C2D_DrawTriangle(pos0.x, pos0.y, colors.color0, pos1.x, pos1.y, colors.color1,
                   pos2.x, pos2.y, colors.color2, 0.5f);
}

void TriangleSolid(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int color) {
  C2D_DrawTriangle(pos0.x, pos0.y, color, pos1.x, pos1.y, color, pos2.x, pos2.y,
                   color, 0.5f);
}

void TriangleLined(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int color,
                   int t) {
  // 3 Line Cqalls lol more efficient than Rect
  C2D_DrawLine(pos0.x, pos0.y, color, pos1.x, pos1.y, color, t, 1.f);
  C2D_DrawLine(pos0.x, pos0.y, color, pos2.x, pos2.y, color, t, 1.f);
  C2D_DrawLine(pos1.x, pos1.y, color, pos2.x, pos2.y, color, t, 1.f);
}

void Text(R7Vec2 pos, const std::string &text, RD7TextFlags flags) {
  // The Start of the C2D Text Hell
  if (!RD7I_FNT_VALID()) return;
  // little patch for a freeze
  if (text.length() < 1) return;
  // Variables
  bool updated_mwh = false;
  if (rd7i_d7_mwh.x == 0.0f) {
    rd7i_d7_mwh.x = rd7i_current_screen ? 400 : 320;
    updated_mwh = true;
  }
  if (rd7i_d7_mwh.y == 0.0f) {
    rd7i_d7_mwh.y = 240;
    updated_mwh = true;
  }
  std::string edit_text = text;
  if (edit_text.substr(text.length() - 1) != "\n")
    edit_text.append("\n");  // Add \n to end if not exist
  int line = 0;

  if (flags & RD7TextFlags_Wrap)
    edit_text = WrapText(text, rd7i_d7_mwh.x - pos.x);

  while (edit_text.find('\n') != edit_text.npos) {
    std::string current_line = edit_text.substr(0, edit_text.find('\n'));
    if (flags & RD7TextFlags_Short)
      current_line = GetShortedText(current_line, rd7i_d7_mwh.x - pos.x);
    R7Vec2 newpos = pos;
    // Check Flags
    R7Vec2 dim = GetTextDimensions(current_line);
    if (flags & RD7TextFlags_AlignRight)
      newpos.x = newpos.x - GetTextDimensions(current_line).x;
    if (flags & RD7TextFlags_AlignMid)  // Offset by inpos
      newpos.x = (rd7i_d7_mwh.x * 0.5) - (dim.x * 0.5) + pos.x;
    if (flags & RD7TextFlags_Scroll) {  // Scroll Text
      if (newpos.x + dim.x > rd7i_d7_mwh.x - newpos.x - 10) {
        if (rd7i_d2_ts.find(current_line) == rd7i_d2_ts.end())
          rd7i_d2_ts[current_line] = 0;
        if (rd7i_d2_mln.find(current_line) == rd7i_d2_mln.end())
          rd7i_d2_mln[current_line] =
              GetShortedText(current_line, rd7i_d7_mwh.x - newpos.x).length();
        rd7i_d2_ts[current_line] += rd7i_dtm * 6;
        if ((int)rd7i_d2_ts[current_line] >= (int)current_line.length()) {
          rd7i_d2_ts[current_line] = 0.0f;
        }
        std::string bcl = current_line;
        current_line = current_line.substr((int)rd7i_d2_ts[current_line],
                                           rd7i_d2_mln[current_line]);
        if (newpos.x + GetTextDimensions(current_line).x <
            rd7i_d7_mwh.x - newpos.x)
          current_line +=
              "|" + bcl.substr(0, rd7i_d2_ts[bcl] - (int)current_line.length());
      }
    }
    if (rd7_debugging) {
      RenderD7::Draw2::Rect(newpos, dim, RenderD7::Color::Hex("#ff0000"));
    }
    C2D_Text c2dtext;
    C2D_TextFontParse(&c2dtext, rd7i_d2_fnt, rd7i_text_buffer,
                      current_line.c_str());
    C2D_TextOptimize(&c2dtext);

    if (flags & RD7TextFlags_Shaddow)  // performance Killer xd
      C2D_DrawText(&c2dtext, C2D_WithColor, newpos.x + 1 + (dim.y * line),
                   newpos.y + 1, 0.5, rd7i_d2_txt_size, rd7i_d2_txt_size,
                   RenderD7::StyleColor(RD7Color_TextDisabled));

    C2D_DrawText(&c2dtext, C2D_WithColor, newpos.x, newpos.y + (dim.y * line),
                 0.5, rd7i_d2_txt_size, rd7i_d2_txt_size,
                 RenderD7::StyleColor(RD7Color_Text));
    edit_text = edit_text.substr(edit_text.find('\n') + 1);
    line++;
  }

  if (updated_mwh) rd7i_d7_mwh = R7Vec2(0, 0);
}
void Image(RenderD7::Image *img, const R7Vec2 &pos, const R7Vec2 &scale) {
  if (img->loaded())
    C2D_DrawImageAt(img->get(), pos.x, pos.y, 0.5f, nullptr, scale.x, scale.y);
}
}  // namespace Draw2
}  // namespace RenderD7
