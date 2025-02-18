#include <map>
#include <renderd7/Hid.hpp>

namespace RenderD7 {
class HidApi {
public:
  HidApi() {}
  ~HidApi() {}

  void setKdown(uint32_t &in) { actions[Hid::Down] = &in; }
  void setKheld(uint32_t &in) { actions[Hid::Held] = &in; }
  void setKup(uint32_t &in) { actions[Hid::Up] = &in; }
  void setKrepeat(uint32_t &in) { actions[Hid::DownRepeat] = &in; }
  void setTouchCoords(R7Vec2 &touch_coords) { touch_pos = &touch_coords; }
  void setJS1Movement(R7Vec2 &mvmt) { js1_mv = &mvmt; }
  void setJS2Movement(R7Vec2 &mvmt) { js2_mv = &mvmt; }
  void bindKey(const std::string &event, uint32_t key) {
    key_bindings[event] = key; // Overrides if existing
  }
  void lock(bool lock) { locked = lock; }

  void clear() {
    // Clears Functionality for 1 Frame
    last_touch_pos = R7Vec2();
    touch_pos[0] = R7Vec2();
    backups[Hid::Down] = 0;
    backups[Hid::Held] = 0;
    backups[Hid::Up] = 0;
    backups[Hid::DownRepeat] = 0;
  }

  bool isEvent(const std::string &event, Hid::Actions action) {
    if (locked)
      return false;
    if (key_bindings.find(event) == key_bindings.end())
      return false; // Unknown Event
    if (backups.find(action) == backups.end())
      return false; // What? NOT Alowed acrion
    if (backups[action] & key_bindings[event])
      return true; // Action contains key as flag
    return false;  // Nothing to do
  }

  R7Vec2 getTouchPos() { return touch_pos[0]; }
  R7Vec2 getLastTouchPos() { return last_touch_pos; }

  void update() {
    last_touch_pos = touch_pos[0];
    for (const auto &it : actions) {
      backups[it.first] = it.second[0];
    }
    if(locked) {
      actions[Hid::Down][0] = 0;
      actions[Hid::Held][0] = 0;
      actions[Hid::Up][0] = 0;
      actions[Hid::DownRepeat][0] = 0;
    }
  }

private:
  std::map<Hid::Actions, uint32_t *> actions;
  std::map<Hid::Actions, uint32_t> backups;
  R7Vec2 *touch_pos = nullptr;
  R7Vec2 *js1_mv = nullptr;
  R7Vec2 *js2_mv = nullptr;

  R7Vec2 last_touch_pos;

  std::map<std::string, uint32_t> key_bindings;
  bool locked = false;
};
static HidApi hid_handler;
namespace Hid {
// Register Functions
// Register Current state values
void RegKeyDown(uint32_t &key_down) { hid_handler.setKdown(key_down); }

void RegKeyHeld(uint32_t &key_held) { hid_handler.setKheld(key_held); }

void RegKeyUp(uint32_t &key_up) { hid_handler.setKup(key_up); }

void RegKeyRepeat(uint32_t &repeat) { hid_handler.setKrepeat(repeat); }

void RegTouchCoords(R7Vec2 &touch_pos) {
  hid_handler.setTouchCoords(touch_pos);
}

void RegAnalog1Movement(R7Vec2 &movement) {
  hid_handler.setJS1Movement(movement);
}

void RegAnalog2Movement(R7Vec2 &movement) {
  hid_handler.setJS2Movement(movement);
}

// Register Keys
void RegKeyEvent(const std::string &event, uint32_t key) {
  hid_handler.bindKey(event, key);
}

bool IsEvent(const std::string &event, Actions action) {
  return hid_handler.isEvent(event, action);
}
R7Vec2 GetTouchPosition() { return hid_handler.getTouchPos(); }
R7Vec2 GetLastTouchPosition() { return hid_handler.getLastTouchPos(); }
void Update() { hid_handler.update(); }
void Lock() { hid_handler.lock(true); }
void Unlock() { hid_handler.lock(false); }
void Clear() { hid_handler.clear(); }
} // namespace Hid
} // namespace RenderD7