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

#include <ctime>
#include <renderd7/Color.hpp>
#include <renderd7/DrawV2.hpp>
#include <renderd7/Hid.hpp>
#include <renderd7/Message.hpp>
#include <renderd7/Overlays.hpp>
#include <renderd7/UI7.hpp>
#include <renderd7/internal_db.hpp>
#include <unordered_map>

template <typename T>
inline T d7max(T a, T b) {
  return a > b ? a : b;
}

template <typename T>
inline T d7min(T a, T b) {
  return a < b ? a : b;
}

// As the 3ds doesn't support std::chrono
#ifdef __3DS__
/// @brief 3ds System Ticks per milli second
/// Already defined in FTrace ik but
/// I Want to make UI7 and Draw2 more
/// Independent of the main RenderD7 api
#define TICKS_PER_MSEC 268111.856
#include <3ds.h>
#define __get_time() (float)svcGetSystemTick() / (float)TICKS_PER_MSEC
#else
#include <chrono>
#define __get_time()                                        \
  std::chrono::duration_cast<std::chrono::milliseconds>(    \
      std::chrono::system_clock().now().time_since_epoch()) \
      .count()
#endif

// Basement structs enums etc
struct UI7ID {
  UI7ID(const std::string &id) {
    real_id = id;
    title = id;
    if (id.substr(0, 2) == "##") {
      title = "";
      has_title = false;
    }
    // Ensure the id is lowercase
    std::transform(real_id.begin(), real_id.end(), real_id.begin(),
                   [](unsigned char c) { return std::tolower(c); });
  }
  UI7ID() {
    title = "";
    real_id = "";
    has_title = false;
  }

  std::string Title() { return title; }

  std::string ID() { return real_id; }

  bool operator==(const UI7ID &in) { return (real_id == in.real_id); }

  bool operator!=(const UI7ID &in) { return !(*this == in); }

  std::string title;
  std::string real_id;
  bool has_title;
};

using DrawCmdType = int;
enum DrawCmdType_ {
  DrawCmdType_Skip,
  DrawCmdType_Rect,
  DrawCmdType_Triangle,
  DrawCmdType_Text,
  DrawCmdType_Debug,
};

struct DrawCmd;
void UI7CtxPushDebugCmd(std::shared_ptr<DrawCmd> ref);

struct DrawCmd {
  DrawCmd(R7Vec4 r, RD7Color c) : rect(r), clr(c), type(DrawCmdType_Rect) {}
  DrawCmd(R7Vec4 r, unsigned int c)
      : rect(r), ovr_clr(c), type(DrawCmdType_Rect) {
    clr = -1;
  }
  DrawCmd(R7Vec4 r, R7Vec2 a, RD7Color c)
      : rect(r), add_coords(a), clr(c), type(DrawCmdType_Triangle) {}
  DrawCmd(R7Vec4 r, R7Vec2 a, unsigned int c)
      : rect(r), add_coords(a), ovr_clr(c), type(DrawCmdType_Triangle) {
    clr = -1;
  }
  DrawCmd(R7Vec2 p, const std::string &t, RD7Color c, RD7TextFlags f = 0)
      : rect(p, R7Vec2()),
        text(t),
        clr(c),
        text_flags(f),
        type(DrawCmdType_Text) {}
  DrawCmd(R7Vec2 p, const std::string &t, unsigned int c, RD7TextFlags f = 0)
      : rect(p, R7Vec2()),
        text(t),
        ovr_clr(c),
        text_flags(f),
        type(DrawCmdType_Text) {
    clr = -1;
  }
  // Empty Command
  DrawCmd() {}
  // For Debug API
  DrawCmd(DrawCmd &thiz) {
    auto cmd = DrawCmd::New();
    cmd->add_coords = thiz.add_coords;
    cmd->clr = thiz.clr;
    cmd->ovr_clr = thiz.ovr_clr;
    cmd->rect = thiz.rect;
    cmd->stype = thiz.type;
    cmd->text = thiz.text;
    cmd->text_box = thiz.text_box;
    cmd->text_flags = thiz.text_flags;
    cmd->type = DrawCmdType_Debug;
    cmd->screen = rd7i_current_screen;
    UI7CtxPushDebugCmd(cmd);
  }
  R7Vec4 rect;              // Position / Size
  R7Vec2 add_coords;        // Additional Coords
  RD7Color clr;             // Color
  std::string text;         // Text
  DrawCmdType type;         // DrawCmd Type
  DrawCmdType stype;        // Second Type
  unsigned int ovr_clr;     // Override Color (if not clr)
  RD7TextFlags text_flags;  // Flags for Text Rendering
  R7Vec2 text_box;          // Maximum text Box
  bool screen;

  // Process the Command
  void Process() {
    DrawCmd(*this);
    if (type == DrawCmdType_Skip) {
      return;
    }
    auto color = (clr == -1 ? ovr_clr : RenderD7::ThemeActive()->Get(clr));
    if (type == DrawCmdType_Rect) {
      RenderD7::Draw2::RFS(R7Vec2(rect.x, rect.y), R7Vec2(rect.z, rect.w),
                           color);
    } else if (type == DrawCmdType_Triangle) {
      RenderD7::Draw2::TriangleSolid(R7Vec2(rect.x, rect.y),
                                     R7Vec2(rect.z, rect.w), add_coords, color);
    } else if (type == DrawCmdType_Text) {
      if (text_box.x || text_box.y) {
        RenderD7::TextMaxBox(text_box);
      }
      RenderD7::Draw2::TextClr(R7Vec2(rect.x, rect.y), text, color, text_flags);
      if (text_box.x || text_box.y) {
        RenderD7::TextDefaultBox();
      }
    } else if (type == DrawCmdType_Debug) {
      Debug();
    }
  }
  void Debug() {
    RenderD7::OnScreen(screen ? Top : Bottom);
    if (stype == DrawCmdType_Skip && type != DrawCmdType_Debug) return;
    // auto color = (clr == -1 ? ovr_clr : RenderD7::ThemeActive()->Get(clr));
    if (stype == DrawCmdType_Rect) {
      RenderD7::Draw2::TriangleLined(
          R7Vec2(rect.x, rect.y), R7Vec2(rect.x + rect.z, rect.y),
          R7Vec2(rect.x, rect.y + rect.w), 0xff0000ff);
      RenderD7::Draw2::TriangleLined(R7Vec2(rect.x + rect.z, rect.y + rect.w),
                                     R7Vec2(rect.x + rect.z, rect.y),
                                     R7Vec2(rect.x, rect.y + rect.w),
                                     0xff0000ff);
    } else if (stype == DrawCmdType_Text) {
      auto szs = RenderD7::GetTextDimensions(text);
      if (text_flags & RD7TextFlags_AlignRight) {
        rect.x -= szs.x;
      }
      RenderD7::Draw2::TriangleLined(
          R7Vec2(rect.x, rect.y), R7Vec2(rect.x + szs.x, rect.y),
          R7Vec2(rect.x, rect.y + szs.y), 0xff00ffff);
      RenderD7::Draw2::TriangleLined(R7Vec2(rect.x + szs.x, rect.y + szs.y),
                                     R7Vec2(rect.x + szs.x, rect.y),
                                     R7Vec2(rect.x, rect.y + szs.y),
                                     0xff00ffff);
    }
  }
  // For Smart Pointer
  using Ref = std::shared_ptr<DrawCmd>;
  template <typename... args>
  static Ref New(args &&...cargs) {
    return std::make_shared<DrawCmd>(std::forward<args>(cargs)...);
  }
};

struct UI7Menu {
  UI7Menu() {}
  UI7ID menuid;                   // menu ID
  R7Vec2 cursor;                  // cursor
  R7Vec2 cb;                      // backup cursor
  R7Vec2 slc;                     // sameline cursor
  float scrolling_offset = 0.f;   //  MenuScrolling Pos
  bool enable_scrolling = false;  // Menu Scrolling
  float scrolling_mod = 0.f;      // For Menu Scrolling effect
  float tbh;                      // TabBar Height
  bool show_scroolbar = true;     // Show Scrollbar
  bool has_touch = false;         // To Disable touch on Top Screen

  // SubMenu
  std::string submenu;

  // DrawLists
  std::vector<DrawCmd::Ref> background;
  std::vector<DrawCmd::Ref> main;
  std::vector<DrawCmd::Ref> front;

  R7Vec2 ms;   // Max Size
  R7Vec2 msr;  // Max Size Real (Slider)
  R7Vec2 mdp;  // Mouse/Touch Initial pos
  // For Smart Pointer
  using Ref = std::shared_ptr<UI7Menu>;
  static Ref New() { return std::make_shared<UI7Menu>(); }
};

struct UI7_Ctx {
  UI7_Ctx() {
    delta = 0.0f;
    time = 0.0f;
    is_activated = false;
    _last = 0;
    in_menu = false;
    debugging = false;
  }
  float delta;
  float time;
  bool is_activated;
  float _last;
  bool in_menu;
  bool debugging;
  std::map<std::string, UI7Menu::Ref> menus;
  std::vector<DrawCmd::Ref> debug_calls;
  UI7Menu::Ref cm;
};

UI7_Ctx *ui7_ctx;

void UI7CtxPushDebugCmd(DrawCmd::Ref ref) {
  ui7_ctx->debug_calls.push_back(ref);
}

bool UI7CtxValidate() {
  if (ui7_ctx == nullptr) return false;
  if (!ui7_ctx->is_activated) return false;
  return true;
}

bool UI7CtxInMenu() {
  if (!UI7CtxValidate()) return false;
  return ui7_ctx->in_menu;
}

bool UI7CtxBeginMenu(const std::string &lb) {
  if (!UI7CtxValidate()) return false;
  auto id = UI7ID(lb);
  if (ui7_ctx->menus.find(id.ID()) == ui7_ctx->menus.end())
    ui7_ctx->menus.insert(std::make_pair(id.ID(), UI7Menu::New()));
  ui7_ctx->cm = ui7_ctx->menus[id.ID()];
  ui7_ctx->cm->menuid = id;
  ui7_ctx->cm->cursor = R7Vec2(0, 0);
  ui7_ctx->cm->has_touch = !rd7i_current_screen;
  ui7_ctx->in_menu = true;
  return true;
}

void UI7CtxEndMenu() {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  RenderD7::Ftrace::ScopedTrace tr("ui7", "EndMenu");
  // Draw Scrollbar
  if (ui7_ctx->cm->enable_scrolling) {
    int sw = (rd7i_current_screen ? 400 : 320);
    int tsp = 5 + ui7_ctx->cm->tbh;
    int szs = 240 - tsp - 5;
    int lszs = 20;  // Lowest Slider size
    float slider_h = (szs - 4) * (static_cast<float>(szs - 4) /
                                  static_cast<float>(ui7_ctx->cm->msr.y));
    int slider_rh = d7min(d7max(slider_h, (float)lszs), (float)(szs - 4));
    int slider_pos = d7min(
        static_cast<float>(tsp + szs - slider_rh - 4),
        d7max(
            static_cast<float>(tsp),
            static_cast<float>(tsp) +
                static_cast<float>(
                    (szs) * (static_cast<float>(ui7_ctx->cm->scrolling_offset) /
                             static_cast<float>(ui7_ctx->cm->msr.y)))));
    int slider_w = 4;
    ui7_ctx->cm->front.push_back(
        DrawCmd::New(R7Vec4(R7Vec2(sw - 12, tsp), R7Vec2(slider_w * 2, szs)),
                     RD7Color_List0));
    ui7_ctx->cm->front.push_back(DrawCmd::New(
        R7Vec4(R7Vec2(sw - 10, slider_pos + 2), R7Vec2(slider_w, slider_rh)),
        RD7Color_Selector));
  }
  // Proccess DrawLists
  for (auto &it : ui7_ctx->cm->background) it->Process();
  for (auto &it : ui7_ctx->cm->main) it->Process();
  for (auto &it : ui7_ctx->cm->front) it->Process();
  ui7_ctx->cm->background.clear();
  ui7_ctx->cm->main.clear();
  ui7_ctx->cm->front.clear();
  ui7_ctx->cm = nullptr;
  ui7_ctx->in_menu = false;
}

void UI7CtxCursorMove(R7Vec2 size) {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->slc = ui7_ctx->cm->cursor + R7Vec2(size.x + 5, 0);
  ui7_ctx->cm->cursor.x = 5;
  ui7_ctx->cm->cursor += R7Vec2(0, size.y + 5);
  ui7_ctx->cm->ms = R7Vec2(ui7_ctx->cm->slc.x, ui7_ctx->cm->cursor.y);
  // TODO: Correct that calculation
  ui7_ctx->cm->msr = R7Vec2(ui7_ctx->cm->slc.x, ui7_ctx->cm->slc.y - 10);
}

namespace UI7 {
bool InBox(R7Vec2 inpos, R7Vec2 boxpos, R7Vec2 boxsize) {
  if ((inpos.x > boxpos.x) && (inpos.y > boxpos.y) &&
      (inpos.x < boxpos.x + boxsize.x) && (inpos.y < boxpos.y + boxsize.y))
    return true;
  return false;
}

void Init() {
  // If Context is valid it makes no sense to reinit lol
  if (UI7CtxValidate()) return;
  ui7_ctx = new UI7_Ctx;
  ui7_ctx->delta = 0.0f;
  ui7_ctx->time = 0.0f;
  ui7_ctx->_last = __get_time();
  ui7_ctx->is_activated = true;
}

void Deinit() {
  // Dont deinit something not initialized
  // Please dont count how often init... was
  // written wrong by me :(
  if (!UI7CtxValidate()) return;
  ui7_ctx->is_activated = false;
  ui7_ctx->menus.clear();
  delete ui7_ctx;
}

void Update() {
  // Dont do anithing without ctx;
  if (!UI7CtxValidate()) return;
  ui7_ctx->debug_calls.clear();
  float current = __get_time();
  ui7_ctx->delta = (current - ui7_ctx->_last) / 1000.f;
  ui7_ctx->_last = current;
  ui7_ctx->time += ui7_ctx->delta;
}

float GetTime() {
  if (!UI7CtxValidate()) return 0;
  return ui7_ctx->time;
}

float GetDeltaTime() {
  if (!UI7CtxValidate()) return 0;
  return ui7_ctx->delta;
}

bool Button(const std::string &label, R7Vec2 size) {
  bool ret = false;
  if (!UI7CtxValidate()) return ret;
  R7Vec2 textdim = RenderD7::GetTextDimensions(label);
  if (size.x == 0) {
    size.x = textdim.x + 8;
  }
  if (size.y == 0) {
    size.y = textdim.y + 4;
  }
  RD7Color btn = RD7Color_Button;
  R7Vec2 pos = GetCursorPos();

  UI7CtxCursorMove(size);

  if (ui7_ctx->cm->enable_scrolling) {
    R7Vec2 pb = pos;
    pos -= R7Vec2(0, ui7_ctx->cm->scrolling_offset);
    if (pos.y > 240 ||
        (pos.y + size.y < ui7_ctx->cm->tbh - 5 && pb.y > ui7_ctx->cm->tbh))
      return false;
  }

  if (ui7_ctx->cm->has_touch) {
    if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
        InBox(RenderD7::Hid::GetLastTouchPosition(), pos, size)) {
      btn = RD7Color_ButtonActive;
      ret = true;
    } else if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Held) &&
               InBox(RenderD7::Hid::GetTouchPosition(), pos, size)) {
      btn = RD7Color_ButtonHovered;
    }
  }

  ui7_ctx->cm->main.push_back(DrawCmd::New(R7Vec4(pos, size), btn));
  pos = R7Vec2(pos.x + size.x * 0.5f - textdim.x * 0.5,
               pos.y + size.y * 0.5f - textdim.y * 0.5);
  ui7_ctx->cm->main.push_back(
      DrawCmd::New(pos, label, RenderD7::ThemeActive()->AutoText(btn)));
  return ret;
}

void Checkbox(const std::string &label, bool &c) {
  if (!UI7CtxValidate()) return;
  float sv = (RenderD7::TextGetSize() * 40) * 0.9;
  R7Vec2 cbs = R7Vec2(sv, sv);
  R7Vec2 txtdim = RenderD7::GetTextDimensions(label);
  R7Vec2 inp = cbs + R7Vec2(txtdim.x + 5, 0);
  RD7Color bg = RD7Color_FrameBg;

  R7Vec2 pos = GetCursorPos();

  UI7CtxCursorMove(inp);

  if (ui7_ctx->cm->enable_scrolling) {
    R7Vec2 pb = pos;
    pos -= R7Vec2(0, ui7_ctx->cm->scrolling_offset);
    if (pos.y > 240 ||
        (pos.y + cbs.y < ui7_ctx->cm->tbh - 5 && pb.y > ui7_ctx->cm->tbh))
      return;
  }

  if (ui7_ctx->cm->has_touch) {
    if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
        InBox(RenderD7::Hid::GetLastTouchPosition(), pos, inp)) {
      bg = RD7Color_FrameBgHovered;
      c = !c;
    } else if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Held) &&
               InBox(RenderD7::Hid::GetTouchPosition(), pos, inp)) {
      bg = RD7Color_FrameBgHovered;
    }
  }

  ui7_ctx->cm->main.push_back(DrawCmd::New(R7Vec4(pos, cbs), bg));
  if (c == true) {
    ui7_ctx->cm->main.push_back(DrawCmd::New(
        R7Vec4(pos + R7Vec2(2, 2), cbs - R7Vec2(4, 4)), RD7Color_Checkmark));
  }
  ui7_ctx->cm->main.push_back(
      DrawCmd::New(pos + R7Vec2(cbs.x + 5, 1), label,
                   RenderD7::ThemeActive()->AutoText(RD7Color_Background)));
}

void Label(const std::string &label, RD7TextFlags flags) {
  if (!UI7CtxValidate()) return;
  R7Vec2 textdim = RenderD7::GetTextDimensions(label);
  R7Vec2 pos = GetCursorPos();
  auto upos = pos;
  // Remove some y offset cause texts have some offset
  UI7CtxCursorMove(textdim - R7Vec2(0, 4));

  if (ui7_ctx->cm->enable_scrolling) {
    R7Vec2 pb = pos;
    pos -= R7Vec2(0, ui7_ctx->cm->scrolling_offset);
    if (pos.y > 240 ||
        (pos.y + textdim.y < ui7_ctx->cm->tbh - 5 && pb.y > ui7_ctx->cm->tbh))
      return;
  }

  float tbh = RenderD7::TextGetSize() * 40;
  auto &list =
      (upos.y + textdim.y < tbh) ? ui7_ctx->cm->front : ui7_ctx->cm->main;

  list.push_back(DrawCmd::New(
      pos, label,
      RenderD7::ThemeActive()->AutoText(
          (upos.y + textdim.y < tbh ? RD7Color_Header : RD7Color_Background)),
      flags));
}

void Progressbar(float value) {
  if (!UI7CtxValidate()) return;
  R7Vec2 pos = GetCursorPos();
  R7Vec2 size = R7Vec2((rd7i_current_screen ? 400 : 320) - (pos.x * 2), 20);
  if (ui7_ctx->cm->show_scroolbar && ui7_ctx->cm->enable_scrolling)
    size.x -= 16;
  UI7CtxCursorMove(size);

  if (ui7_ctx->cm->enable_scrolling) {
    R7Vec2 pb = pos;
    pos -= R7Vec2(0, ui7_ctx->cm->scrolling_offset);
    if (pos.y > 240 ||
        (pos.y + size.y < ui7_ctx->cm->tbh - 5 && pb.y > ui7_ctx->cm->tbh))
      return;
  }

  ui7_ctx->cm->main.push_back(
      DrawCmd::New(R7Vec4(pos, size), RD7Color_FrameBg));
  ui7_ctx->cm->main.push_back(
      DrawCmd::New(R7Vec4(pos + R7Vec2(2, 2), size - R7Vec2(4, 4)),
                   RD7Color_FrameBgHovered));
  if (!(value != value) && !(value < 0.0) && !(value > 1.0)) {
    ui7_ctx->cm->main.push_back(DrawCmd::New(
        R7Vec4(pos + R7Vec2(2, 2), R7Vec2((size.x - 4) * value, size.y - 4)),
        RD7Color_Progressbar));
  }
}

void Image(RenderD7::Image *img) {
  if (!UI7CtxValidate()) return;
  R7Vec2 pos = GetCursorPos();
  UI7CtxCursorMove(R7Vec2(img->get_size().x, img->get_size().y));

  if (ui7_ctx->cm->enable_scrolling) {
    R7Vec2 pb = pos;
    pos -= R7Vec2(0, ui7_ctx->cm->scrolling_offset);
    if (pos.y > 240 || (pos.y + img->get_size().y < ui7_ctx->cm->tbh - 5 &&
                        pb.y > ui7_ctx->cm->tbh))
      return;
  }

  RenderD7::Draw2::Image(img, pos);
}

void BrowserList(const std::vector<std::string> &entrys, int &selection,
                 RD7TextFlags txtflags, R7Vec2 size, int max_entrys) {
  if (!UI7CtxValidate()) return;
  if (selection < 0) return;
  float tmp_txt = RenderD7::TextGetSize();
  RenderD7::TextDefaultSize();
  R7Vec2 pos = GetCursorPos();
  if (pos.y + 15 * max_entrys > 230) max_entrys = (int)((230 - pos.y) / 15);
  if (size.x == 0) size.x = (rd7i_current_screen ? 400 : 320) - (pos.x * 2);
  if (size.y == 0) size.y = (max_entrys * 15);
  UI7CtxCursorMove(size);
  for (int i = 0; i < max_entrys; i++) {
    ui7_ctx->cm->main.push_back(
        DrawCmd::New(R7Vec4(pos + R7Vec2(0, 15 * i), R7Vec2(size.x, 15)),
                     (i % 2 == 0 ? RD7Color_List0 : RD7Color_List1)));
  }
  for (size_t i = 0;
       i < ((entrys.size() < (size_t)max_entrys) ? entrys.size()
                                                 : (size_t)max_entrys);
       i++) {
    int selindex = (selection < max_entrys ? selection : (max_entrys - 1));
    int list_index =
        (selection < max_entrys ? i : (i + selection - (max_entrys - 1)));
    if (i == (size_t)selindex) {
      ui7_ctx->cm->main.push_back(DrawCmd::New(
          R7Vec4(pos + R7Vec2(0, 15 * i), R7Vec2(size.x, 15)),
          (unsigned int)RenderD7::Color::RGBA(RD7Color_Selector)
              .fade_to(RD7Color_SelectorFade, std::sin(RenderD7::GetTime()))
              .toRGBA()));
    }
    auto cmd =
        DrawCmd::New(pos + R7Vec2(5, 15 * i), entrys[list_index],
                     RenderD7::ThemeActive()->AutoText(
                         selindex == (int)i
                             ? RD7Color_Selector
                             : (i % 2 == 0 ? RD7Color_List0 : RD7Color_List1)),
                     txtflags | RD7TextFlags_Short);
    cmd->text_box = R7Vec2(size.x, 15);
    ui7_ctx->cm->main.push_back(cmd);
  }
  RenderD7::CustomTextSize(tmp_txt);
}

void InputText(const std::string &label, std::string &text,
               const std::string &hint) {
  if (!UI7CtxValidate()) return;
  float sv = (RenderD7::TextGetSize() * 40) * 0.9;
  R7Vec2 cbs = R7Vec2(144, sv);
  R7Vec2 txtdim = RenderD7::GetTextDimensions(label);
  R7Vec2 inp = cbs + R7Vec2(txtdim.x + 5, 0);
  RD7Color bg = RD7Color_FrameBg;
  auto id = UI7ID(label);
  RD7KeyboardState kbd_state;  // tmp (goes out of scope)

  R7Vec2 pos = GetCursorPos();
  UI7CtxCursorMove(inp);

  if (ui7_ctx->cm->enable_scrolling) {
    R7Vec2 pb = pos;
    pos -= R7Vec2(0, ui7_ctx->cm->scrolling_offset);
    if (pos.y > 240 ||
        (pos.y + cbs.y < ui7_ctx->cm->tbh - 5 && pb.y > ui7_ctx->cm->tbh))
      return;
  }

  if (ui7_ctx->cm->has_touch) {
    if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
        InBox(RenderD7::Hid::GetLastTouchPosition(), pos, inp)) {
      bg = RD7Color_FrameBgHovered;
      RenderD7::AddOvl(
          std::make_unique<RenderD7::Ovl_Keyboard>(text, kbd_state, hint));
    } else if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Held) &&
               InBox(RenderD7::Hid::GetTouchPosition(), pos, inp)) {
      bg = RD7Color_FrameBgHovered;
    }
  }

  ui7_ctx->cm->main.push_back(DrawCmd::New(R7Vec4(pos, cbs), bg));
  ui7_ctx->cm->main.push_back(
      DrawCmd::New(pos + R7Vec2(5, 1), (text != "" ? text : hint),
                   RenderD7::ThemeActive()->AutoText(bg)));
  ui7_ctx->cm->main.push_back(
      DrawCmd::New(pos + R7Vec2(cbs.x + 5, 1), id.Title(),
                   RenderD7::ThemeActive()->AutoText(RD7Color_Background)));
}

bool BeginMenu(const std::string &title, R7Vec2 size, UI7MenuFlags flags) {
  if (!UI7CtxValidate()) return false;
  if (UI7CtxInMenu()) return false;
  auto id = UI7ID(title);
  auto ret = UI7CtxBeginMenu(title);
  if (!ret) return ret;
  bool titlebar = true;
  if (size.x == 0) {
    size.x = rd7i_current_screen ? 400 : 320;
  }
  if (size.y == 0) {
    size.y = 240;
  }
  RD7TextFlags txtflags = 0;
  float tbh = RenderD7::TextGetSize() * 40;
  ui7_ctx->cm->tbh = tbh;

  if (flags & UI7MenuFlags_NoTitlebar) {
    titlebar = false;
    ui7_ctx->cm->tbh = 0.f;
  }
  if (flags & UI7MenuFlags_TitleMid) txtflags = RD7TextFlags_AlignMid;
  ui7_ctx->cm->enable_scrolling = (flags & UI7MenuFlags_Scrolling);
  if (ui7_ctx->cm->enable_scrolling && !rd7i_current_screen) {
    // Patch that sets scrolling to 0 if max pos is not out of screen
    if (ui7_ctx->cm->scrolling_offset != 0.f && ui7_ctx->cm->ms.y < 235) {
      ui7_ctx->cm->scrolling_offset = 0.f;
    }
    // Auto scroll back if last object is on screen
    if (ui7_ctx->cm->scrolling_offset > ui7_ctx->cm->ms.y - 240 &&
        ui7_ctx->cm->ms.y != 0 && ui7_ctx->cm->ms.y >= 235) {
      ui7_ctx->cm->scrolling_offset -= 3;
      // Patch to Scroll to perfect pos
      if (ui7_ctx->cm->scrolling_offset < ui7_ctx->cm->ms.y - 240) {
        ui7_ctx->cm->scrolling_offset = ui7_ctx->cm->ms.y - 240;
      }
    }
    // Auto Scroll back if offset gets below 0
    if (ui7_ctx->cm->scrolling_offset < 0) {
      ui7_ctx->cm->scrolling_offset += 3;
      if (ui7_ctx->cm->scrolling_offset > 0) ui7_ctx->cm->scrolling_offset = 0;
    }

    // Zero out scrolling_mod if it goeas < -40
    // or > 40 over the max size
    if (ui7_ctx->cm->scrolling_offset < -40 ||
        ui7_ctx->cm->scrolling_offset > ui7_ctx->cm->ms.y - 200) {
      ui7_ctx->cm->scrolling_mod = 0.f;
    }
    if (ui7_ctx->cm->has_touch) {
      if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Down)) {
        // Set the mdp Value as Start Pos
        ui7_ctx->cm->mdp = RenderD7::Hid::GetTouchPosition();
      } else if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up)) {
        // 0 out the start pos
        ui7_ctx->cm->mdp = R7Vec2();
      }
      if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Held)) {
        // Set modifier
        auto np = RenderD7::Hid::GetTouchPosition();
        // Check if and do nothing if the scrolling ofset goes out of screen
        if (ui7_ctx->cm->scrolling_offset < ui7_ctx->cm->ms.y - 200 &&
            ui7_ctx->cm->scrolling_offset > -40) {
          float cursor_mod = (ui7_ctx->cm->mdp.y - np.y);
          if (ui7_ctx->cm->scrolling_mod <= 4.f &&
              ui7_ctx->cm->scrolling_mod >= -4 && cursor_mod != 0.0f) {
            if (cursor_mod > 0) {
              ui7_ctx->cm->scrolling_mod = cursor_mod;
            } else {
              ui7_ctx->cm->scrolling_mod = cursor_mod;
            }
          }
        }
        // Update Start pos
        ui7_ctx->cm->mdp = np;
      }
    }
    // New Scrolling efect
    if (ui7_ctx->cm->scrolling_mod != 0)
      ui7_ctx->cm->scrolling_offset += ui7_ctx->cm->scrolling_mod;
    // Slow out the effect
    if (ui7_ctx->cm->scrolling_mod < 0.f) {
      ui7_ctx->cm->scrolling_mod += 0.4f;
      if (ui7_ctx->cm->scrolling_mod > 0.f) {
        ui7_ctx->cm->scrolling_mod = 0.f;
      }
    } else if (ui7_ctx->cm->scrolling_mod > 0.f) {
      ui7_ctx->cm->scrolling_mod -= 0.4f;
      if (ui7_ctx->cm->scrolling_mod < 0.f) {
        ui7_ctx->cm->scrolling_mod = 0.f;
      }
    }
  } else {
    // Set scrollingoffset and mod to 0 if not scrolling enabled
    ui7_ctx->cm->scrolling_offset = 0.f;
    ui7_ctx->cm->scrolling_mod = 0.f;
  }

  ui7_ctx->cm->background.push_back(
      DrawCmd::New(R7Vec4(R7Vec2(), size), RD7Color_Background));
  if (titlebar) {
    ui7_ctx->cm->front.push_back(
        DrawCmd::New(R7Vec4(R7Vec2(), R7Vec2(size.x, tbh)), RD7Color_Header));
    ui7_ctx->cm->front.push_back(DrawCmd::New(
        R7Vec2(5, 2), id.Title(),
        RenderD7::ThemeActive()->AutoText(RD7Color_Header), txtflags));
  }

  SetCursorPos(R7Vec2(5, ui7_ctx->cm->tbh + 5));

  return ret;
}

void EndMenu() { UI7CtxEndMenu(); }

void Grid(const std::string &name, const R7Vec2 &size, const R7Vec2 &entry_size,
          void (*display_func)(void *, R7Vec2), void **data_array,
          size_t num_entrys) {
  if (!UI7CtxValidate()) return;
  if (num_entrys <= 0) return;
  if (data_array == nullptr) return;
  R7Vec2 pos = GetCursorPos();
  R7Vec2 cpos(pos);

  int neh = std::floor(size.x / (entry_size.x + 4));
  int nev = std::floor(size.y / (entry_size.y + 4));

  // Inside Grid Offset
  R7Vec2 igoff = R7Vec2();
  if (neh >= 2 && nev >= 2) {
    igoff = R7Vec2(
        ((size.x) / 2 - (((neh - 1) * (entry_size.x + 4)) + entry_size.x) / 2),
        ((size.y) / 2 - ((nev - 1) * ((entry_size.y + 4)) + entry_size.y) / 2));
  }
  // Y-Offset
  int yoff = 0;

  pos += igoff;
  for (size_t i = 0; i < num_entrys; i++) {
    display_func(data_array[i], pos);
    if (ui7_ctx->debugging)
      RenderD7::Draw2::Text(pos + R7Vec2(4, 4), std::to_string(i));
    if (pos.x + (entry_size.x * 2) > (cpos.x + size.x) &&
        pos.y + (entry_size.y * 2) > cpos.y + size.y) {
      break;
    } else if (pos.x + (entry_size.x * 2) > (cpos.x + size.x)) {
      pos = R7Vec2(5 + igoff.x, pos.y + entry_size.y + 4);
      yoff++;
    } else {
      pos += R7Vec2(entry_size.x + 4, 0);
    }
  }

  UI7CtxCursorMove(size);
}

void ColorSelector(const std::string &label, unsigned int &color) {
  if (!UI7CtxValidate()) return;
  float sv = (RenderD7::TextGetSize() * 40) * 0.9;
  R7Vec2 cbs = R7Vec2(sv, sv);
  R7Vec2 txtdim = RenderD7::GetTextDimensions(label);
  R7Vec2 inp = cbs + R7Vec2(txtdim.x + 5, 0);
  auto outline =
      RenderD7::Color::RGBA(color).is_light() ? 0xff000000 : 0xffffffff;
  auto id = UI7ID(label);

  R7Vec2 pos = GetCursorPos();
  UI7CtxCursorMove(inp);

  if (ui7_ctx->cm->enable_scrolling) {
    R7Vec2 pb = pos;
    pos -= R7Vec2(0, ui7_ctx->cm->scrolling_offset);
    if (pos.y > 240 ||
        (pos.y + cbs.y < ui7_ctx->cm->tbh - 5 && pb.y > ui7_ctx->cm->tbh))
      return;
  }

  if (ui7_ctx->cm->has_touch) {
    if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
        InBox(RenderD7::Hid::GetLastTouchPosition(), pos, inp)) {
      ui7_ctx->cm->submenu = id.ID();
      // Nullify scrolling mod to fix freeze
      ui7_ctx->cm->scrolling_mod = 0.0f;
      RenderD7::Hid::Lock();
      RenderD7::Hid::Clear();
    }
  }

  if (ui7_ctx->cm->submenu == id.ID()) {
    // Base
    auto clr = RenderD7::Color::RGBA(color);
    // Keyboard logic
    static bool inkbd;
    static std::string kbd_txt;
    static RD7KeyboardState kbd_state;
    static int kbd_rgba = 0;
    if (kbd_state == RD7KeyboardState_Confirm) {
      int num = std::atoi(kbd_txt.c_str());
      if (num < 0 || num > 255) {
        RenderD7::PushMessage("UI7", "Error: Value must\nbe 0 - 255!");
      } else {
        if (kbd_rgba == 1) {
          clr.m_r = num;
        } else if (kbd_rgba == 2) {
          clr.m_g = num;
        } else if (kbd_rgba == 3) {
          clr.m_b = num;
        } else if (kbd_rgba == 4) {
          clr.m_a = num;
        }
      }
    }
    if (kbd_state) {
      inkbd = false;
      kbd_state = RD7KeyboardState_None;
      kbd_txt = "";
      kbd_rgba = 0;
    }

    if (!inkbd) RenderD7::Hid::Unlock();
    bool isunlock = false;
    R7Vec2 npos = pos;
    if (npos.y < ui7_ctx->cm->tbh + 2) npos.y = ui7_ctx->cm->tbh;
    if (npos.y + 97 > 235) npos.y = 137;
    // Input
    if (ui7_ctx->cm->has_touch) {
      auto ltp = RenderD7::Hid::GetLastTouchPosition();
      if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
          (InBox(ltp, npos + R7Vec2(2, 2), cbs) ||
           !InBox(ltp, npos, R7Vec2(107, 97)))) {
        ui7_ctx->cm->submenu = "";
        isunlock = true;
      } else if (RenderD7::Hid::IsEvent("cancel", RenderD7::Hid::Down)) {
        ui7_ctx->cm->submenu = "";
        isunlock = true;
        RenderD7::Hid::Clear();
      }
      if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up)) {
        if (InBox(ltp, npos + R7Vec2(2, cbs.y * 3 + 4), R7Vec2(50, cbs.y))) {
          inkbd = true;
          kbd_txt = std::to_string(clr.m_r);
          kbd_rgba = 1;
          RenderD7::AddOvl(std::make_unique<RenderD7::Ovl_Keyboard>(
              kbd_txt, kbd_state, "", RD7Keyboard_Numpad));
        } else if (InBox(ltp, npos + R7Vec2(54, cbs.y * 3 + 4),
                         R7Vec2(50, cbs.y))) {
          inkbd = true;
          kbd_txt = std::to_string(clr.m_g);
          kbd_rgba = 2;
          RenderD7::AddOvl(std::make_unique<RenderD7::Ovl_Keyboard>(
              kbd_txt, kbd_state, "", RD7Keyboard_Numpad));
        } else if (InBox(ltp, npos + R7Vec2(2, cbs.y * 4 + 4),
                         R7Vec2(50, cbs.y))) {
          inkbd = true;
          kbd_txt = std::to_string(clr.m_b);
          kbd_rgba = 3;
          RenderD7::AddOvl(std::make_unique<RenderD7::Ovl_Keyboard>(
              kbd_txt, kbd_state, "", RD7Keyboard_Numpad));
        } else if (InBox(ltp, npos + R7Vec2(54, cbs.y * 4 + 4),
                         R7Vec2(50, cbs.y))) {
          inkbd = true;
          kbd_txt = std::to_string(clr.m_a);
          kbd_rgba = 4;
          RenderD7::AddOvl(std::make_unique<RenderD7::Ovl_Keyboard>(
              kbd_txt, kbd_state, "", RD7Keyboard_Numpad));
        }
      }
      color = clr.toRGBA();
    }
    if (!isunlock && !inkbd) RenderD7::Hid::Lock();
    // Draw Frame
    ui7_ctx->cm->front.push_back(
        DrawCmd::New(R7Vec4(npos.x, npos.y, 107, 97), RD7Color_FrameBg));
    // Draw Color Button
    ui7_ctx->cm->front.push_back(
        DrawCmd::New(R7Vec4(npos + R7Vec2(2, 2), cbs), outline));
    ui7_ctx->cm->front.push_back(
        DrawCmd::New(R7Vec4(npos + R7Vec2(4, 4), cbs - R7Vec2(4, 4)), color));
    // Setup TextCommand with Short flag
    auto cmd =
        DrawCmd::New(npos + R7Vec2(cbs.x + 7, 1), label,
                     RenderD7::ThemeActive()->AutoText(RD7Color_FrameBg));
    cmd->text_box = R7Vec2(93 - cbs.x - 5, 0);
    cmd->text_flags |= RD7TextFlags_Short;
    ui7_ctx->cm->front.push_back(cmd);
    // Add luminance text
    ui7_ctx->cm->front.push_back(DrawCmd::New(
        npos + R7Vec2(2, cbs.y + 4), "lum: " + std::to_string(clr.luminance()),
        RenderD7::ThemeActive()->AutoText(RD7Color_FrameBg)));
    // Add Hex value
    ui7_ctx->cm->front.push_back(
        DrawCmd::New(npos + R7Vec2(2, cbs.y * 2 + 4),
                     "hex: " + RenderD7::Color::RGBA2Hex(color),
                     RenderD7::ThemeActive()->AutoText(RD7Color_FrameBg)));
    // Red
    {
      ui7_ctx->cm->front.push_back(DrawCmd::New(
          R7Vec4(npos + R7Vec2(2, cbs.y * 3 + 4), R7Vec2(50, cbs.y)),
          RD7Color_FrameBgHovered));
      ui7_ctx->cm->front.push_back(
          DrawCmd::New(R7Vec4(npos + R7Vec2(2, cbs.y * 3 + 4),
                              R7Vec2(50 * ((float)clr.m_r / 255.f), cbs.y)),
                       0xff0000ff));

      auto ncmd = DrawCmd::New(npos + R7Vec2(2, cbs.y * 3 + 4),
                               std::to_string(clr.m_r), RD7Color_Text);
      ncmd->text_flags |= RD7TextFlags_AlignMid;
      ncmd->text_box = R7Vec2(50, 0);
      ui7_ctx->cm->front.push_back(ncmd);
    }
    // Green
    {
      ui7_ctx->cm->front.push_back(DrawCmd::New(
          R7Vec4(npos + R7Vec2(54, cbs.y * 3 + 4), R7Vec2(50, cbs.y)),
          RD7Color_FrameBgHovered));
      ui7_ctx->cm->front.push_back(
          DrawCmd::New(R7Vec4(npos + R7Vec2(54, cbs.y * 3 + 4),
                              R7Vec2(50 * ((float)clr.m_g / 255.f), cbs.y)),
                       0xff00ff00));
      auto ncmd = DrawCmd::New(npos + R7Vec2(54, cbs.y * 3 + 4),
                               std::to_string(clr.m_g), RD7Color_Text);
      ncmd->text_flags |= RD7TextFlags_AlignMid;
      ncmd->text_box = R7Vec2(50, 0);
      ui7_ctx->cm->front.push_back(ncmd);
    }
    // Blue
    {
      ui7_ctx->cm->front.push_back(DrawCmd::New(
          R7Vec4(npos + R7Vec2(2, cbs.y * 4 + 4), R7Vec2(50, cbs.y)),
          RD7Color_FrameBgHovered));
      ui7_ctx->cm->front.push_back(
          DrawCmd::New(R7Vec4(npos + R7Vec2(2, cbs.y * 4 + 4),
                              R7Vec2(50 * ((float)clr.m_b / 255.f), cbs.y)),
                       0xffff0000));

      auto ncmd = DrawCmd::New(npos + R7Vec2(2, cbs.y * 4 + 4),
                               std::to_string(clr.m_b), RD7Color_Text);
      ncmd->text_flags |= RD7TextFlags_AlignMid;
      ncmd->text_box = R7Vec2(50, 0);
      ui7_ctx->cm->front.push_back(ncmd);
    }
    // Alpha
    {
      ui7_ctx->cm->front.push_back(DrawCmd::New(
          R7Vec4(npos + R7Vec2(54, cbs.y * 4 + 4), R7Vec2(50, cbs.y)),
          RD7Color_FrameBgHovered));
      ui7_ctx->cm->front.push_back(
          DrawCmd::New(R7Vec4(npos + R7Vec2(54, cbs.y * 4 + 4),
                              R7Vec2(50 * ((float)clr.m_a / 255.f), cbs.y)),
                       0xffffffff));

      auto ncmd = DrawCmd::New(npos + R7Vec2(54, cbs.y * 4 + 4),
                               std::to_string(clr.m_a), RD7Color_Text);
      ncmd->text_flags |= RD7TextFlags_AlignMid;
      ncmd->text_box = R7Vec2(50, 0);
      ui7_ctx->cm->front.push_back(ncmd);
    }
  }

  ui7_ctx->cm->main.push_back(DrawCmd::New(R7Vec4(pos, cbs), outline));
  ui7_ctx->cm->main.push_back(
      DrawCmd::New(R7Vec4(pos + R7Vec2(2, 2), cbs - R7Vec2(4, 4)), color));
  ui7_ctx->cm->main.push_back(
      DrawCmd::New(pos + R7Vec2(cbs.x + 5, 1), label,
                   RenderD7::ThemeActive()->AutoText(RD7Color_Background)));
}

bool BeginTree(const std::string &text) {
  // TODO
  return false;
}

void EndTree() {
  // TODO
}

R7Vec2 GetCursorPos() {
  if (!UI7CtxValidate()) return R7Vec2();
  if (!UI7CtxInMenu()) return R7Vec2();
  return ui7_ctx->cm->cursor;
}

void SetCursorPos(R7Vec2 cp) {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->cb = ui7_ctx->cm->cursor;
  ui7_ctx->cm->cursor = cp;
}

void RestoreCursor() {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->cursor = ui7_ctx->cm->cb;
}

void SameLine() {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->cursor = ui7_ctx->cm->slc;
}

void Debug() {
  if (!UI7CtxValidate()) return;
  if (ui7_ctx->debugging) {
    for (auto &it : ui7_ctx->debug_calls) {
      it->Process();
    }
  }
}

float GetScrollingOffset() {
  if (!UI7CtxValidate()) return 0.f;
  if (!UI7CtxInMenu()) return 0.f;
  return ui7_ctx->cm->scrolling_offset;
}

bool &IsDebugging() {
  if (!UI7CtxValidate()) {
    // Return a Default Val
    static bool t = false;
    return t;
  }
  return ui7_ctx->debugging;
}

}  // namespace UI7