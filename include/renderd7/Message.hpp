#pragma once

#include <string>

namespace RenderD7 {
struct Message {
  Message(std::string t, std::string m) {
    title = t;
    message = m;
    animationframe = 0;
  }

  std::string title;
  std::string message;
  int animationframe;
};

void ProcessMessages();
void PushMessage(const Message& msg);
// Config
void SetMessageIdleStartFrame(int frame);
void SetMessageTotalAnimationFrames(int total_frames);
void SetMessageFadeOutStartFrame(int frame);
}  // namespace RenderD7