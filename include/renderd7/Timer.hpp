#pragma once

#include <3ds.h>

#include <renderd7/smart_ctor.hpp>

namespace RenderD7 {
class Timer {
 public:
  Timer(bool autostart = true);
  ~Timer() {}
  RD7_SMART_CTOR(Timer)
  void Reset();
  void Tick();
  void Pause();
  void Resume();
  float Get();
  float GetLive();
  bool Running();

 private:
  uint64_t last = 0;
  uint64_t current = 0;
  bool is_running = false;
};
}  // namespace RenderD7