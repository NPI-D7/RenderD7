/// WARNING
/// THIS IS BETA STUFF
/// ITS MAKE LIKE EXTERNAL BUT
/// FOR RENDERD7 ITS INTEGRATED

#pragma once

#include <renderd7/R7Vec.hpp>
#include <string>

namespace RenderD7 {
namespace Hid {
enum Actions {
  Down = 0,
  Held = 1,
  Up = 2,
  DownRepeat = 3,
};
// Register Functions
// Register Current state values
void RegKeyDown(uint32_t &key_down);
void RegKeyHeld(uint32_t &key_held);
void RegKeyUp(uint32_t &key_up);
void RegKeyRepeat(uint32_t &repeat);
void RegTouchCoords(R7Vec2 &touch_pos);
// Not Corectly Implemented Yet
void RegAnalog1Movement(R7Vec2 &movement);
void RegAnalog2Movement(R7Vec2 &movement);
// Register Keys
void RegKeyEvent(const std::string &event, uint32_t key);
// KeyEvents
bool IsEvent(const std::string &event, Actions action);
R7Vec2 GetTouchPosition();
R7Vec2 GetLastTouchPosition();
void Update();
// Lock/Unlock Input api for example for Keyboard
void Lock();
void Unlock();
void Clear();
} // namespace Hid
} // namespace RenderD7