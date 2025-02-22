#include <citro2d.h>

#include <renderd7/Render2.hpp>
#include <renderd7/internal_db.hpp>

namespace RenderD7 {
const float R2::default_text_size = 0.5f;
float R2::text_size = 0.5;
Font::Ref R2::font;
std::map<std::string, float> R2::ts;
std::map<std::string, int> R2::mln;
bool R2::next_lined = false;
std::vector<R2::R2Cmd::Ref> R2::commands;
R2Screen R2::current_screen = R2Screen_Bottom;

void R2::Init() { R2::font = Font::New(); }

void R2::SetFont(Font::Ref fnt) {
  if (!fnt) return;
  R2::font = fnt;
}

Font::Ref R2::GetFont() { return R2::font; }

void R2::DefaultFont() { R2::font->Unload(); }

void R2::DrawNextLined() { R2::next_lined = true; }

void R2::OnScreen(R2Screen screen) {
  if (screen < 0 || screen > R2Screen_Top) return;
  R2::current_screen = screen;
}

void R2::SetTextSize(float szs) { text_size = szs; }

void R2::DefaultTextSize() { text_size = R2::default_text_size; }

float R2::GetTextSize() { return text_size; }

R2Screen R2::GetCurrentScreen() { return current_screen; }

R7Vec2 R2::GetTextDimensions(const std::string& text) {
  C2D_TextBufClear(rd7i_d2_dimbuf);
  float w = 0, h = 0;
  C2D_Text c2dtext;
  C2D_TextFontParse(&c2dtext, font->Ptr(), rd7i_d2_dimbuf, text.c_str());
  C2D_TextGetDimensions(&c2dtext, R2::text_size, R2::text_size, &w, &h);
  return R7Vec2(w, h);
}

std::string R2::WrapText(const std ::string& in, int maxlen) {
  std::string out;
  std::string line;
  int line_x = 0;
  std::istringstream istream(in);
  std::string temp;

  while (istream >> temp) {
    R7Vec2 dim = R2::GetTextDimensions(line + temp);
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

std::string R2::ShortText(const std::string& in, int maxlen) {
  auto textdim = R2::GetTextDimensions(in);
  if (textdim.x < (float)maxlen) return in;
  std::string ft = "";
  std::string worker = in;
  if (in.find_last_of('.') != in.npos) {
    ft = in.substr(in.find_last_of('.'));
    worker = in.substr(0, in.find_last_of('.'));
  }

  maxlen -= R2::GetTextDimensions(ft).x - R2::GetTextDimensions("(...)").x;
  float len_mod = (float)maxlen / textdim.x;
  int pos = (in.length() * len_mod) / rd7_draw2_tsm;
  std::string out;

  out = in.substr(0, pos);

  for (size_t i = pos; i < worker.length(); i++) {
    out += worker[i];
    if (R2::GetTextDimensions(out + "(...)" + ft).x > (float)maxlen) {
      out += "(...)";
      out += ft;
      return out;
    }
  }
  return out;  // Impossible to reach
}

R7Vec2 R2::GetCurrentScreenSize() {
  return R7Vec2(R2::current_screen == R2Screen_Bottom ? 320 : 400, 240);
}

// Main Processing of Draw Calls
void R2::Process() {
  for (auto& it : R2::commands) {
    if (it->type <= 0 || it->type > 6) {
      // Skip
      continue;
    }
    C2D_SceneBegin(it->Screen ? rd7_top : rd7_bottom);
    if (it->type == 1) {
      // Rect
      if (it->lined) {
        C2D_DrawLine(it->pos.x, it->pos.y, it->clr, it->pos.x + it->pszs.x,
                     it->pos.y, it->clr, 1.f, 0.5f);
        C2D_DrawLine(it->pos.x, it->pos.y, it->clr, it->pos.x,
                     it->pos.y + it->pszs.y, it->clr, 1.f, 0.5f);
        C2D_DrawLine(it->pos.x + it->pszs.x, it->pos.y, it->clr,
                     it->pos.x + it->pszs.x, it->pos.y + it->pszs.y, it->clr,
                     1.f, 0.5f);
        C2D_DrawLine(it->pos.x, it->pos.y + it->pszs.y, it->clr,
                     it->pos.x + it->pszs.x, it->pos.y + it->pszs.y, it->clr,
                     1.f, 0.5f);
      } else {
        C2D_DrawRectSolid(it->pos.x, it->pos.y, 0.5, it->pszs.x, it->pszs.y,
                          it->clr);
      }
    } else if (it->type == 2) {
      // Triangle
      if (it->lined) {
        C2D_DrawLine(it->pos.x, it->pos.y, it->clr, it->pszs.x, it->pszs.y,
                     it->clr, 1, 0.5f);
        C2D_DrawLine(it->pos.x, it->pos.y, it->clr, it->ap.x, it->ap.y, it->clr,
                     1, 0.5f);
        C2D_DrawLine(it->pszs.x, it->pszs.y, it->clr, it->ap.x, it->ap.y,
                     it->clr, 1, 0.5f);
      } else {
        C2D_DrawTriangle(it->pos.x, it->pos.y, it->clr, it->pszs.x, it->pszs.y,
                         it->clr, it->ap.x, it->ap.y, it->clr, 0.5);
      }
    } else if (it->type == 3) {
      // Text
      // little patch for a freeze
      if (it->text.length() < 1) continue;
      if (it->pszs.x == 0.0f) {
        it->pszs.x = it->Screen == R2Screen_Top ? 400 : 320;
      }
      if (it->pszs.y == 0.0f) {
        it->pszs.y = 240;
      }
      std::string edit_text = it->text;
      if (edit_text.substr(it->text.length() - 1) != "\n")
        edit_text.append("\n");  // Add \n to end if not exist
      int line = 0;

      if (it->flags & RD7TextFlags_Wrap) {
        edit_text = WrapText(it->text, it->pszs.x - it->pos.x);
      }

      while (edit_text.find('\n') != edit_text.npos) {
        std::string current_line = edit_text.substr(0, edit_text.find('\n'));
        if (it->flags & RD7TextFlags_Short)
          current_line = R2::ShortText(current_line, it->pszs.x - it->pos.x);
        R7Vec2 newpos = it->pos;
        // Check Flags
        R7Vec2 dim = R2::GetTextDimensions(current_line);
        if (it->flags & RD7TextFlags_AlignRight) newpos.x = newpos.x - dim.x;
        if (it->flags & RD7TextFlags_AlignMid)  // Offset by inpos
          newpos.x = (it->pszs.x * 0.5) - (dim.x * 0.5) + it->pos.x;
        if (it->flags & RD7TextFlags_Scroll) {  // Scroll Text
          // Look into Old Draw2 Code
          // TODO: Create Code for this
        }
        if (rd7i_debugging) {
          R2::DrawNextLined();
          R2::AddRect(newpos, dim, 0xff0000ff);
        }
        C2D_Text c2dtext;
        C2D_TextFontParse(&c2dtext, font->Ptr(), rd7i_text_buffer,
                          current_line.c_str());
        C2D_TextOptimize(&c2dtext);

        if (it->flags & RD7TextFlags_Shaddow)  // performance Killer xd
          C2D_DrawText(&c2dtext, C2D_WithColor, newpos.x + 1 + (dim.y * line),
                       newpos.y + 1, 0.5, R2::text_size, R2::text_size,
                       RenderD7::ThemeActive()->Get(RD7Color_TextDisabled));

        C2D_DrawText(&c2dtext, C2D_WithColor, newpos.x,
                     newpos.y + (dim.y * line), 0.5, R2::text_size,
                     R2::text_size, it->clr);
        edit_text = edit_text.substr(edit_text.find('\n') + 1);
        line++;
      }
    } else if (it->type == 4) {
      if (it->img->Loadet()) {
        C2D_DrawImageAt(it->img->Get(), it->pos.x, it->pos.y, 0.5f);
      }
    } else if (it->type == 5) {
      // TODO: Move the Draw Func into this API
      it->spr->Draw();
    } else if (it->type == 6) {
      C2D_DrawLine(it->pos.x, it->pos.y, it->clr, it->pszs.x, it->pszs.y,
                   it->clr, it->ap.x, 0.5f);
    }
  }
  R2::commands.clear();
}

void R2::AddRect(R7Vec2 pos, R7Vec2 size, RD7Color clr) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->pszs = size;
  cmd->clr = RenderD7::ThemeActive()->Get(clr);
  cmd->type = 1;  // Rect
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddRect(R7Vec2 pos, R7Vec2 size, unsigned int clr) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->pszs = size;
  cmd->clr = clr;
  cmd->type = 1;  // Rect
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddLine(R7Vec2 pos_a, R7Vec2 pos_b, RD7Color clr, int t) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos_a;
  cmd->pszs = pos_b;
  cmd->ap.x = t;
  cmd->clr = RenderD7::ThemeActive()->Get(clr);
  cmd->type = 6;  // Line
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddLine(R7Vec2 pos_a, R7Vec2 pos_b, unsigned int clr, int t) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos_a;
  cmd->pszs = pos_b;
  cmd->ap.x = t;
  cmd->clr = clr;
  cmd->type = 6;  // Line
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, RD7Color clr) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos0;
  cmd->pszs = pos1;
  cmd->ap = pos2;
  cmd->clr = RenderD7::ThemeActive()->Get(clr);
  cmd->type = 2;  // Triangle
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int clr) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos0;
  cmd->pszs = pos1;
  cmd->ap = pos2;
  cmd->clr = clr;
  cmd->type = 2;  // Triangle
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddText(R7Vec2 pos, const std::string& text, RD7Color clr,
                 RD7TextFlags flags, R7Vec2 tmb) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->pszs = tmb;
  cmd->clr = RenderD7::ThemeActive()->Get(clr);
  cmd->flags = flags;
  cmd->text = text;
  cmd->type = 3;  // Text
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddText(R7Vec2 pos, const std::string& text, unsigned int clr,
                 RD7TextFlags flags, R7Vec2 tmb) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->pszs = tmb;
  cmd->clr = clr;
  cmd->flags = flags;
  cmd->text = text;
  cmd->type = 3;  // Text
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddImage(R7Vec2 pos, Image::Ref img) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->img = img;
  cmd->type = 4;  // Image
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddSprite(Sprite::Ref spr) {
  auto cmd = R2Cmd::New();
  cmd->spr = spr;
  cmd->type = 5;  // Sprite
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

}  // namespace RenderD7