#include <ctime>
#include <renderd7/Color.hpp>
#include <renderd7/DrawV2.hpp>
#include <renderd7/Hid.hpp>
#include <renderd7/Overlays.hpp>
#include <renderd7/UI7.hpp>
#include <renderd7/internal_db.hpp>
#include <unordered_map>

// As the 3ds doesn't support std::chrono
#ifdef __3DS__
/// @brief 3ds System Ticks per milli second
/// Already defined in FTrace ik but
/// I Want to make UI7 and Draw2 more
/// Independent of the main RenderD7 api
#define TICKS_PER_MSEC 268111.856
#include <3ds.h>
#define __get_time() svcGetSystemTick() / TICKS_PER_MSEC
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
    lt = time(0);
  }
  UI7ID() {
    title = "";
    real_id = "";
    has_title = false;
    lt = time(0);
  }

  std::string Title() {
    lt = time(0);
    return title;
  }

  std::string ID() {
    lt = time(0);
    return real_id;
  }

  bool operator==(const UI7ID &in) { return (real_id == in.real_id); }

  bool operator!=(const UI7ID &in) { return !(*this == in); }

  std::string title;
  std::string real_id;
  bool has_title;
  int lt;
};

struct UI7OBJ {
  UI7OBJ() {}
  UI7OBJ(const R7Vec4 &i0, const int &i1) {
    box = i0;
    type = i1;
    s = rd7i_current_screen;
  }
  void Debug() {
    RenderD7::OnScreen(s ? Top : Bottom);
    RenderD7::Draw2::TriangleLined(R7Vec2(box.x, box.y),
                                   R7Vec2(box.x + box.z, box.y),
                                   R7Vec2(box.x, box.y + box.w), 0xff0000ff);
    RenderD7::Draw2::TriangleLined(
        R7Vec2(box.x, box.y + box.w), R7Vec2(box.x + box.z, box.y),
        R7Vec2(box.x + box.z, box.y + box.w), 0xff0000ff);
  }
  R7Vec4 box;
  int type;
  bool s = false;
};

std::vector<unsigned int> ui7i_debug_colors{
    0x00000000,
};

struct UI7_Ctx {
  UI7_Ctx() {
    delta = 0.0f;
    time = 0.0f;
    is_activated = false;
    _last = 0;
    cursor = R7Vec2();
    slc = R7Vec2();
    cbackup = R7Vec2();
    in_menu = false;
    current_menu = UI7ID("");
    debugging = true;
  }
  float delta;
  float time;
  bool is_activated;
  unsigned long long _last;
  R7Vec2 cursor;
  R7Vec2 cbackup;
  R7Vec2 slc;
  bool in_menu;
  bool debugging;
  UI7ID current_menu;
  std::map<std::string, R7Vec2> grid_mapping;
  std::unordered_map<std::string, UI7ID *> ids;
  std::vector<UI7OBJ> objects;
};

UI7_Ctx *ui7_ctx;

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
  if (UI7CtxInMenu()) return false;
  ui7_ctx->current_menu = UI7ID(lb);
  ui7_ctx->in_menu = true;
  return true;
}

void UI7CtxEndMenu() {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->current_menu = UI7ID("");
  ui7_ctx->in_menu = false;
}

void UI7CtxCursorMove(R7Vec2 size) {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->slc = ui7_ctx->cursor + R7Vec2(5, 0);
  ui7_ctx->cursor.x = 5;
  ui7_ctx->cursor += R7Vec2(0, size.y + 5);
  ui7_ctx->slc += R7Vec2(size.x, 0);
}

void UI7CtxRegObj(const UI7OBJ &obj) {
  if (!UI7CtxValidate()) return;
  if (!ui7_ctx->debugging) return;
  ui7_ctx->objects.push_back(obj);
}

UI7ID *UI7CtxNewID(const std::string &i) {
  std::string t = i;
  std::transform(t.begin(), t.end(), t.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if (ui7_ctx->ids.find(t) != ui7_ctx->ids.end()) {
    return ui7_ctx->ids[t];
  }
  auto id = new UI7ID(i);
  ui7_ctx->ids[id->real_id] = id;
  return id;
}

void UI7CtxClearIDs() {
  for (auto it = ui7_ctx->ids.begin(); it != ui7_ctx->ids.end();) {
    if (it->second == nullptr) {
      ++it;
      continue;
    }
    if (time(0) - it->second->lt > 5) {
      delete it->second;
      it = ui7_ctx->ids.erase(it);
      continue;
    }
    ++it;
  }
}

int rd7i_ui7_ids_active() {
  if (!UI7CtxValidate()) return 0;
  return ui7_ctx->ids.size();
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
  ui7_ctx->cursor = R7Vec2(0, 0);
  ui7_ctx->is_activated = true;
}

void Deinit() {
  // Dont deinit something not initialized
  // Please dont count how often init... was
  // written wront by me :(
  if (!UI7CtxValidate()) return;
  ui7_ctx->is_activated = false;
  delete ui7_ctx;
}

void Update() {
  // Dont do anithing without ctx;
  if (!UI7CtxValidate()) return;
  unsigned long long current = __get_time();
  ui7_ctx->delta = (float)((float)current - (float)ui7_ctx->_last) / 1000.f;
  ui7_ctx->_last = current;
  ui7_ctx->time += ui7_ctx->delta;
  ui7_ctx->cursor = R7Vec2();
  UI7CtxClearIDs();
  if (ui7_ctx->debugging) ui7_ctx->objects.clear();
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
  UI7CtxRegObj(UI7OBJ(R7Vec4(pos, size), 0));

  if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
      InBox(RenderD7::Hid::GetLastTouchPosition(), pos, size)) {
    btn = RD7Color_ButtonActive;
    ret = true;
  } else if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Held) &&
             InBox(RenderD7::Hid::GetTouchPosition(), pos, size)) {
    btn = RD7Color_ButtonHovered;
  }

  RenderD7::Draw2::RFS(pos, size, RenderD7::StyleColor(btn));
  pos = R7Vec2(pos.x + size.x * 0.5f - textdim.x * 0.5,
               pos.y + size.y * 0.5f - textdim.y * 0.5);
  RenderD7::TextColorByBg(btn);
  RenderD7::Draw2::Text(pos, label);
  RenderD7::UndoColorEdit(RD7Color_Text);
  UI7CtxCursorMove(size);
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

  if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
      InBox(RenderD7::Hid::GetLastTouchPosition(), pos, inp)) {
    bg = RD7Color_FrameBgHovered;
    c = !c;
  } else if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Held) &&
             InBox(RenderD7::Hid::GetTouchPosition(), pos, inp)) {
    bg = RD7Color_FrameBgHovered;
  }

  RenderD7::Draw2::RFS(pos, cbs, RenderD7::StyleColor(bg));
  if (c == true) {
    RenderD7::Draw2::RFS(pos + R7Vec2(2, 2), cbs - R7Vec2(4, 4),
                         RenderD7::StyleColor(RD7Color_Checkmark));
    UI7CtxRegObj(UI7OBJ(R7Vec4(pos + R7Vec2(2, 2), cbs - R7Vec2(4, 4)), 2));
  }
  RenderD7::Draw2::Text(pos + R7Vec2(cbs.x + 5, 1), label);
  UI7CtxRegObj(UI7OBJ(R7Vec4(pos, cbs + R7Vec2(txtdim.x + 5, 0)), 0));
  UI7CtxRegObj(UI7OBJ(R7Vec4(pos, cbs), 1));
}

void Label(const std::string &label, RD7TextFlags flags) {
  if (!UI7CtxValidate()) return;
  R7Vec2 textdim = RenderD7::GetTextDimensions(label);
  R7Vec2 pos = GetCursorPos();
  float tbh = RenderD7::TextGetSize() * 40;
  if (flags & RD7TextFlags_AlignRight) {
    UI7CtxRegObj(UI7OBJ(R7Vec4(pos - R7Vec2(textdim.x, 0), textdim), 0));
  } else {
    UI7CtxRegObj(UI7OBJ(R7Vec4(pos, textdim), 0));
  }
  // Remove some y offset cause texts have some offset
  UI7CtxCursorMove(textdim - R7Vec2(0, 4));
  RenderD7::TextColorByBg(
      (pos.y + textdim.y < tbh ? RD7Color_Header : RD7Color_Background));
  RenderD7::Draw2::Text(pos, label, flags);
  RenderD7::UndoColorEdit(RD7Color_Text);
}

void Progressbar(float value) {
  if (!UI7CtxValidate()) return;
  R7Vec2 pos = GetCursorPos();
  R7Vec2 size = R7Vec2((rd7i_current_screen ? 400 : 320) - (pos.x * 2), 20);
  UI7CtxCursorMove(size);

  RenderD7::Draw2::RFS(pos, size, RenderD7::StyleColor(RD7Color_FrameBg));
  RenderD7::Draw2::RFS(pos + R7Vec2(2, 2), size - R7Vec2(4, 4),
                       RenderD7::StyleColor(RD7Color_FrameBgHovered));
  if (!(value != value) && !(value < 0.0) && !(value > 1.0))
    RenderD7::Draw2::RFS(pos + R7Vec2(2, 2),
                         R7Vec2((size.x - 4) * value, size.y - 4),
                         RenderD7::StyleColor(RD7Color_Progressbar));
}

void Image(RenderD7::Image *img) {
  if (!UI7CtxValidate()) return;
  R7Vec2 pos = GetCursorPos();
  UI7CtxCursorMove(R7Vec2(img->get_size().x, img->get_size().y));
  UI7CtxRegObj(UI7OBJ(R7Vec4(pos, img->get_size()), 0));

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
    RenderD7::Draw2::RFS(
        pos + R7Vec2(0, 15 * i), R7Vec2(size.x, 15),
        RenderD7::StyleColor((i % 2) == 0 ? RD7Color_List0 : RD7Color_List1));
  }
  for (size_t i = 0;
       i < ((entrys.size() < (size_t)max_entrys) ? entrys.size()
                                                 : (size_t)max_entrys);
       i++) {
    int selindex = (selection < max_entrys ? selection : (max_entrys - 1));
    int list_index =
        (selection < max_entrys ? i : (i + selection - (max_entrys - 1)));
    if (i == (size_t)selindex) {
      RenderD7::Draw2::RectFilled(
          pos + R7Vec2(0, 15 * i), R7Vec2(size.x, 15),
          {RenderD7::StyleColor(RD7Color_Selector),
           RenderD7::Color::RGBA(RD7Color_Selector)
               .fade_to(RD7Color_SelectorFade, std::sin(RenderD7::GetTime()))
               .toRGBA(),
           RenderD7::StyleColor(RD7Color_Selector),
           RenderD7::Color::RGBA(RD7Color_Selector)
               .fade_to(RD7Color_SelectorFade, std::sin(RenderD7::GetTime()))
               .toRGBA()});
      RenderD7::TextColorByBg(RD7Color_Selector);
      RenderD7::TextMaxBox(R7Vec2(size.x, 0));
      RenderD7::Draw2::Text(pos + R7Vec2(5, 15 * i), entrys[list_index],
                            txtflags | RD7TextFlags_Short);
      RenderD7::TextDefaultBox();
      RenderD7::UndoColorEdit(RD7Color_Text);
    } else {
      RenderD7::Ftrace::Beg("app", "short_algo");
      RenderD7::TextColorByBg(RD7Color_List0);
      RenderD7::TextMaxBox(R7Vec2(size.x, 0));
      RenderD7::Draw2::Text(pos + R7Vec2(5, 15 * i), entrys[list_index],
                            txtflags | RD7TextFlags_Short);
      RenderD7::TextDefaultBox();
      RenderD7::UndoColorEdit(RD7Color_Text);
      RenderD7::Ftrace::End("app", "short_algo");
    }
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
  auto id = UI7CtxNewID(label);

  R7Vec2 pos = GetCursorPos();

  UI7CtxCursorMove(inp);

  if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Up) &&
      InBox(RenderD7::Hid::GetLastTouchPosition(), pos, inp)) {
    bg = RD7Color_FrameBgHovered;
    RenderD7::AddOvl(std::make_unique<RenderD7::Ovl_Keyboard>(text, hint));
  } else if (RenderD7::Hid::IsEvent("touch", RenderD7::Hid::Held) &&
             InBox(RenderD7::Hid::GetTouchPosition(), pos, inp)) {
    bg = RD7Color_FrameBgHovered;
  }

  RenderD7::Draw2::RFS(pos, cbs, RenderD7::StyleColor(bg));
  RenderD7::TextColorByBg(bg);
  RenderD7::Draw2::Text(pos + R7Vec2(5, 1), (text != "" ? text : hint));
  RenderD7::UndoColorEdit(RD7Color_Text);
  RenderD7::Draw2::Text(pos + R7Vec2(cbs.x + 5, 1), id->Title());
}

bool BeginMenu(const std::string &title, R7Vec2 size, UI7MenuFlags flags) {
  if (!UI7CtxValidate()) return false;
  auto id = UI7CtxNewID(title);
  bool titlebar = true;
  if (size.x == 0) {
    size.x = rd7i_current_screen ? 400 : 320;
  }
  if (size.y == 0) {
    size.y = 240;
  }
  RD7TextFlags txtflags = 0;
  float tbh = RenderD7::TextGetSize() * 40;
  if (flags & UI7MenuFlags_NoTitlebar) titlebar = false;
  if (flags & UI7MenuFlags_TitleMid) txtflags = RD7TextFlags_AlignMid;
  RenderD7::Draw2::RFS(R7Vec2(0, 0), size,
                       RenderD7::StyleColor(RD7Color_Background));
  if (titlebar) {
    RenderD7::Draw2::RFS(R7Vec2(0, 0), R7Vec2(size.x, tbh),
                         RenderD7::StyleColor(RD7Color_Header));
    RenderD7::TextColorByBg(RD7Color_Header);
    RenderD7::Draw2::Text(R7Vec2(5, 2), id->title, txtflags);
    RenderD7::UndoColorEdit(RD7Color_Text);
  }

  SetCursorPos(R7Vec2(5, tbh + 5));

  return UI7CtxBeginMenu(title);
}

void EndMenu() { UI7CtxEndMenu(); }

void Grid(const std::string &name, const R7Vec2 &size,
          R7Vec2 (*display_func)(void *, R7Vec2), void **data_array,
          size_t num_entrys) {
  if (!UI7CtxValidate()) return;
  if (num_entrys <= 0) return;
  if (data_array == nullptr) return;
  if (ui7_ctx->grid_mapping.find(UI7ID(name).real_id) ==
      ui7_ctx->grid_mapping.end())
    ui7_ctx->grid_mapping[UI7ID(name).real_id] =
        R7Vec2();  // Register id not exist
  R7Vec2 pos = GetCursorPos();
  R7Vec2 cpos(pos);

  // Debug Grid
  RenderD7::Draw2::Rect(pos, size, RenderD7::Color::Hex("#ff0000"), 1);

  // Y-Offset
  int yoff = 0;

  pos += ui7_ctx->grid_mapping[UI7ID(name).real_id];
  for (size_t i = 0; i < num_entrys; i++) {
    R7Vec2 szs = display_func(data_array[i], pos);
    RenderD7::Draw2::Text(pos + R7Vec2(4, 4), std::to_string(i));
    if (pos.x + (szs.x * 2) > (cpos.x + size.x) &&
        pos.y + szs.y > cpos.y + size.y) {
      if (ui7_ctx->grid_mapping[UI7ID(name).real_id].y == 0)
        ui7_ctx->grid_mapping[UI7ID(name).real_id].y =
            (cpos.y + size.y) / 2 - ((yoff + 1) * (szs.y + 4)) / 2;
      break;
    } else if (pos.x + (szs.x * 2) > (cpos.x + size.x)) {
      if (ui7_ctx->grid_mapping[UI7ID(name).real_id].x == 0)
        ui7_ctx->grid_mapping[UI7ID(name).real_id].x =
            (cpos.x + size.x) / 2 - ((i + 1) * (szs.x + 4)) / 2;
      pos = R7Vec2(5 + ui7_ctx->grid_mapping[UI7ID(name).real_id].x,
                   pos.y + szs.y + 4);
      yoff++;
    } else {
      pos += R7Vec2(szs.x + 4, 0);
    }
  }

  UI7CtxCursorMove(size);
}

bool BeginTree(const std::string &text) { return false; }

void EndTree() {}

R7Vec2 GetCursorPos() {
  if (!UI7CtxValidate()) return R7Vec2();
  return ui7_ctx->cursor;
}

void SetCursorPos(R7Vec2 cp) {
  if (!UI7CtxValidate()) return;
  ui7_ctx->cbackup = ui7_ctx->cursor;
  ui7_ctx->cursor = cp;
}

void RestoreCursor() {
  if (!UI7CtxValidate()) return;
  ui7_ctx->cursor = ui7_ctx->cbackup;
}

void SameLine() {
  if (!UI7CtxValidate()) return;
  ui7_ctx->cursor = ui7_ctx->slc;
}

void Debug() {
  if (!UI7CtxValidate()) return;
  if (ui7_ctx->debugging) {
    for (size_t i = 0; i < ui7_ctx->objects.size(); i++) {
      ui7_ctx->objects[i].Debug();
    }
  }
}

}  // namespace UI7