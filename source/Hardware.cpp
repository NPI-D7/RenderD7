#include <renderd7/Hardware.hpp>
#include <renderd7/internal_db.hpp>

// Os Specific includes
#include <3ds.h>

void RenderD7::Hardware::Initialisize() {
  mcuHwcInit();
  atexit(mcuHwcExit);
  ptmuInit();
  atexit(ptmuExit);
}

bool RenderD7::Hardware::IsHeadphones() {
  if (rd7i_is_ndsp) {
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

int RenderD7::Hardware::GetBatteryPercentage() {
  uint8_t percentLevel = 0;
  MCUHWC_GetBatteryLevel(&percentLevel);
  return percentLevel;
}

float RenderD7::Hardware::Get3dSliderLevel() { return osGet3DSliderState(); }

float RenderD7::Hardware::GetSoundSliderLevel() {
  uint8_t percentLevel;
  MCUHWC_GetSoundSliderLevel(&percentLevel);
  return (float)percentLevel / 100;
}

int RenderD7::Hardware::GetWifiLevel() { return osGetWifiStrength(); }