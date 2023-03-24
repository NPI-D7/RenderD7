#include <renderd7/Hardware.hpp>
#include <renderd7/Security.hpp>

// Os Specific includes
#include <3ds.h>

// RenderD7 Security
extern bool isndspinit;

void RenderD7::Hardware::Initialisize() {
  rd7_security->SafeInit(mcuHwcInit, &mcuHwcExit);
  rd7_security->SafeInit(ptmuInit, &ptmuExit);
}

bool RenderD7::Hardware::IsHeadphones() {
  if (isndspinit) {
    bool inserted;
    DSP_GetHeadphoneStatus(&inserted);
    return inserted;
  } else
    return false;
}

bool RenderD7::Hardware::IsCharging() {
  uint8_t var;
  PTMU_GetBatteryChargeState(&var);
  // Some Security Stuff
  if (var < 0x00 && var > 0x01) {
    return false;
  }
  return (var == 0x01 ? true : false);
}

float RenderD7::Hardware::GetBatteryPercentage() {
  uint8_t percentLevel;
  PTMU_GetBatteryLevel(&percentLevel);
  return (float)percentLevel / 100;
}

float RenderD7::Hardware::Get3dSliderLevel() { return osGet3DSliderState(); }

float RenderD7::Hardware::GetSoundSliderLevel() {
  uint8_t percentLevel;
  MCUHWC_GetSoundSliderLevel(&percentLevel);
  return (float)percentLevel / 100;
}