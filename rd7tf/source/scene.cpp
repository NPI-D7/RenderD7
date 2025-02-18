#include "scene.hpp"

#include <cmath>

static float MakeOffset(float x) {
  float y = cos(x) * 42;
  return y - floor(y);
}

static void Wave(int index, R7Vec2 position, R7Vec2 size, float time,
                 bool dbg) {
  float offset = MakeOffset(index) * 62;
  float x_position =
      position.x + size.x / 8 * ((index % 11) - 1) + cos(offset + time) * 10;
  float y_position = position.y + size.y / 8 * (index / 11) + 40 +
                     sin(offset + time) * 10 + 30;
  float color_effect = 1 - exp(-(index / 11) / 3.0f);

  // As this is 3ds dont go out of the box
  int shrink = 0;
  if (y_position >= position.y + size.y - 90) {
    shrink = y_position - (position.y + size.y - 90);
  }
  // Just to make sure...
  y_position = std::min(y_position, position.y + size.y - (90 - shrink));

  if (dbg)
    RenderD7::Draw2::TriangleLined(
        R7Vec2(x_position, y_position),
        R7Vec2(x_position + 300, y_position + (90 - shrink)),
        R7Vec2(x_position - 300, y_position + (90 - shrink)),
        RenderD7::Color::RGBA(.94f - .17f * color_effect,
                              .61f - .25f * color_effect,
                              .36f + .38f * color_effect)
            .toRGBA());
  else
    RenderD7::Draw2::TriangleSolid(
        R7Vec2(x_position, y_position),
        R7Vec2(x_position + 300, y_position + (90 - shrink)),
        R7Vec2(x_position - 300, y_position + (90 - shrink)),
        RenderD7::Color::RGBA(.94f - .17f * color_effect,
                              .61f - .25f * color_effect,
                              .36f + .38f * color_effect)
            .toRGBA());
}

void DrawWave(R7Vec2 position, R7Vec2 size, float time, bool dbg) {
  RenderD7::Draw2::RectFilledSolid(position, size, 0xff64c9fd);
  int i = 0;
  for (; i < 44; i++) Wave(i, position, size, time, dbg);
}

R7Vec2 testv2 = R7Vec2(48, 48);

R7Vec2 grid_display_entry(void* data, R7Vec2 pos) {
  auto img = reinterpret_cast<RenderD7::Image*>(data);
  RenderD7::Draw2::Image(img, pos);
  RenderD7::Draw2::RFS(pos, testv2, RenderD7::Color::Hex("#0000ff", 140));
  return testv2;
}

std::vector<RenderD7::Image*> imgdb;

Sample::Sample() {
  /*RenderD7::Image* im = new RenderD7::Image();
  im->load("romfs:/icons/icon.png");
  for(int i = 0; i < 64; i++) 
    imgdb.push_back(im);*/
  auto t = RenderD7::FileSystem::GetDirContent("sdmc:/music/");
  for(const auto& it : t)
    names.push_back(it.path);
}

Sample::~Sample() {
  // Here you can clear your data
}

void Sample::Draw() const {
  // Draw Things to Screen:
  // Step 1 -> Select Screen
  RenderD7::OnScreen(Top);
  // Step 2 -> Draw Things
  // The hbloader Triangle Wave
  DrawWave(R7Vec2(0, 0), R7Vec2(400, 240), RenderD7::GetTime(),
           debug_background);
  // For Example A Rect with Draw2 and StyleColorApi
  // And the RFS Wrapper for RectFilledSolid lol
  /*RenderD7::Draw2::RFS(R7Vec2(0, 0), R7Vec2(400, 20),
                       RenderD7::StyleColor(RD7Color_Header));
  // As the Top bar is Dark you need TextColor2
  RenderD7::RedirectColor(RD7Color_Text, RD7Color_Text2);
  RenderD7::Draw2::Text(R7Vec2(5, 2), "RenderD7 - Test Framework");
  RenderD7::Draw2::Text(R7Vec2(395, 2), RENDERD7VSTRING,
                        RD7TextFlags_AlignRight);
  RenderD7::UndoColorEdit(RD7Color_Text);*/
  if (UI7::BeginMenu("RenderD7 Test Framework")) {
    UI7::SetCursorPos(R7Vec2(395, 2));
    UI7::Label(RenderD7::FormatBytes(RenderD7::Memory::GetCurrent()), RD7TextFlags_AlignRight);
    UI7::RestoreCursor();
    //UI7::Grid("Files", R7Vec2(390, 210), grid_display_entry, reinterpret_cast<void**>(imgdb.data()), imgdb.size());
    UI7::BrowserList(names, sel);
    UI7::EndMenu();
  }
  RenderD7::OnScreen(Bottom);
  if (UI7::BeginMenu("Control Center")) {
    if (UI7::Button("RenderD7 Settings"))
      shared_requests[1U] = 1U;  // Request a Toggle
    UI7::SameLine();
    if (UI7::Button("Test Message"))
      shared_requests[2U] = 1U;  // Request Test Msg
    UI7::Checkbox("Debug BG", debug_background);
    UI7::SameLine();
    UI7::Checkbox("RD7-Debug", rd7_debugging);
    UI7::InputText("Search", search__, "Tap Here");
    UI7::EndMenu();
  }
}

void Sample::Logic() {
  for (const auto& it : shared_requests) {
    if (it.first == 1U) {
      if (it.second) RenderD7::LoadSettings();
    } else if (it.first == 2U) {
      if (it.second)
        RenderD7::PushMessage(RenderD7::Message(
            "Test Message", "Button Bressed\nBest Msg Handler..."));
    }
  }
  // Make sure to clear after processing!!!;
  shared_requests.clear();
  if (d7_hDown & KEY_B) debug_background = !debug_background;
}