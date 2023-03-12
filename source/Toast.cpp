#include <renderd7/Draw.hpp>
#include <renderd7/Toast.hpp>

RenderD7::Toast::Toast(std::string head, std::string msg) {
  this->head = head;
  this->msg = msg;
}

void RenderD7::Toast::Draw(void) const {
  RenderD7::OnScreen(Top);
  RenderD7::Draw::Rect(0, msgposy, 400, 70, RenderD7::Color::Hex("#111111"));
  RenderD7::Draw::Rect(0, msgposy, 400, 25, RenderD7::Color::Hex("#222222"));
  RenderD7::Draw::Text(2, msgposy + 3, 0.7f, RenderD7::Color::Hex("#ffffff"),
                       head);
  RenderD7::Draw::Text(2, msgposy + 30, 0.6f, RenderD7::Color::Hex("#ffffff"),
                       msg);
}

void RenderD7::Toast::Logic() {
  this->delay++ /*=1*(int)RenderD7::GetDeltaTime()*/;
  if (msgposy > 170 && delay < 2 * 60)
    msgposy-- /*=(int)RenderD7::GetDeltaTime()*/;

  if (delay >= 5 * 60) {
    msgposy++ /*=(int)RenderD7::GetDeltaTime*/;
    if (msgposy > 400)
      this->Kill();
  }
}