#include <renderd7/Timer.hpp>

// Ticks per MSEC
#define TPMS 268111.856

namespace RenderD7 {
Timer::Timer(bool autostart) {
  if (autostart) is_running = true;
  last = svcGetSystemTick();
  current = last;
}

void Timer::Reset() {
  last = svcGetSystemTick();
  current = last;
}

void Timer::Tick() {
  if (is_running) current = svcGetSystemTick();
}

void Timer::Pause() { is_running = false; }

void Timer::Resume() { is_running = true; }

bool Timer::Running() { return is_running; }

float Timer::Get() { return (float)((current - last) / TPMS); }

float Timer::GetLive() { return (float)((svcGetSystemTick() - last) / TPMS); }
}  // namespace RenderD7