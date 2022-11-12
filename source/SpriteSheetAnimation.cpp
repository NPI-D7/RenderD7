#include <renderd7/SpriteAnimation.hpp>
#include <renderd7/log.hpp>

extern Log renderd7log;

RenderD7::SpriteSheetAnimation::SpriteSheetAnimation() {
  renderd7log.Write("SpriteSheetAnimation createt!");
}
RenderD7::SpriteSheetAnimation::~SpriteSheetAnimation() {
  //
}

void RenderD7::SpriteSheetAnimation::Setup(RenderD7::Sheet *sheet,
                                           size_t imagecount, size_t startimage,
                                           float frame_begin,
                                           float frame_finish) {
  D_totaltime = frame_begin;
  renderd7log.Write("frame_begin success");
  this->images = imagecount;
  renderd7log.Write("imagecount success");
  this->sheet = sheet;
  renderd7log.Write("sheet success");
  this->time = frame_finish;
  renderd7log.Write("frame_finish success");
  RenderD7::SpriteSheetAnimation::FromSheet(this->sheet, startimage);
}
void RenderD7::SpriteSheetAnimation::Play(float timespeed) {
  D_totaltime += timespeed;
  if (D_totaltime >= time) {
    D_totaltime -= time;
    imgs++;
    if (imgs == images) {
      imgs = 0;
    }
  }
  RenderD7::SpriteSheetAnimation::FromSheet(sheet, imgs);
  // RenderD7::SpriteSheetAnimation::Draw();
}