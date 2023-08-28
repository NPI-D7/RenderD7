#include <renderd7/Color.hpp>
#include <renderd7/Draw.hpp>
#include <renderd7/Screen.hpp>
#include <renderd7/ToastsV2.hpp>

#include <algorithm>
#include <memory>
#include <vector>

extern bool rd7_debugging;

std::vector<std::shared_ptr<RenderD7::Message>> msg_lst;
int fade_outs = 200; // Start of fadeout
int idles = 60;      // start of Idle
int anim_len = 300;  // Full Length of Animation

std::pair<int, int> MakePos(int frame, int entry) {
  float fol = anim_len - fade_outs;
  if (frame > fade_outs)
    return std::make_pair(5, 240 - ((entry + 1) * 55) - 5 +
                                 (float)((frame - fade_outs) / fol) * -20);
  if (frame > idles)
    return std::make_pair(5, 240 - ((entry + 1) * 55) - 5);
  return std::make_pair(-150 + ((float)(frame / (float)idles) * 155),
                        240 - ((entry + 1) * 55) - 5);
}

namespace RenderD7 {
float GetDeltaTime(); // Extern from renderd7.cpp

void ProcessMessages() {
  // Draw in ovl mode
  RenderD7::OnScreen(Top);
  float fol = anim_len - fade_outs;
  std::reverse(msg_lst.begin(), msg_lst.end());
  for (size_t i = 0; i < msg_lst.size(); i++) {
    std::pair<int, int> pos = MakePos(msg_lst[i]->animationframe, i);
    if ((pos.second + 150) < 0) {
      // Dont Render Out of Screen
      // And as thay aren't relevant anymore
      // Thay get deleted!
      msg_lst.erase(msg_lst.begin() + i);
    } else {

      int new_alpha = 200;
      if (msg_lst[i]->animationframe > fade_outs) {
        new_alpha =
            200 - (float(msg_lst[i]->animationframe - fade_outs) / fol) * 200;
      }
      u32 txtcol = RenderD7::Color::Hex("#ffffff", new_alpha);
      RenderD7::Draw::Rect(pos.first, pos.second, 150, 50,
                           RenderD7::Color::Hex("#333333", new_alpha));
      RenderD7::Draw::Text(pos.first + 5, pos.second + 1, 0.4f, txtcol,
                           msg_lst[i]->title);
      RenderD7::Draw::Text(pos.first + 5, pos.second + 17, 0.4f, txtcol,
                           msg_lst[i]->message);
      if (rd7_debugging)
        RenderD7::Draw::Text(pos.first + 160, pos.second + 1, 0.5f,
                             RenderD7::Color::Hex("#000000"),
                             std::to_string(msg_lst[i]->animationframe));
      // Why Frameadd? because Message uses int as frame and
      // It seems that lower 0.5 will be rounded to 0
      // Why not replace int with float ?
      // cause of buggy positions (seen in Flappy Bird 3ds for example)
      float frameadd = 60.f * RenderD7::GetDeltaTime();
      // 60fps animation * delta to not slowdown
      // Oh and fix for Startup lol
      // Todo: Only do this on AppStart
      if (msg_lst[i]->animationframe == 0) {
        msg_lst[i]->animationframe += 1;
      } else {
        msg_lst[i]->animationframe += (frameadd < 1.f ? 1.f : frameadd);
      }
      if (msg_lst[i]->animationframe > anim_len) {
        msg_lst.erase(msg_lst.begin() + i);
      }
    }
  }
  // ReReverse ?? lol
  // Cause otherwise the Toasts will swap
  std::reverse(msg_lst.begin(), msg_lst.end());
}

void PushMessage(const Message &msg) {
  msg_lst.push_back(std::make_shared<RenderD7::Message>(msg));
}

void SetIdleStartFrame(int frame) { idles = frame; }

void SetTotalAnimationFrames(int total_frames) { anim_len = total_frames; }

void SetFadeOutStartFrame(int frame) { fade_outs = frame; }
} // namespace RenderD7