#include <renderd7/FunctionTrace.hpp>
#include <renderd7/Ovarlays.hpp>
#include <renderd7/renderd7.hpp>

namespace RenderD7 {
Ovl_Ftrace::Ovl_Ftrace() {}

void Ovl_Ftrace::Draw(void) const { RenderD7::OnScreen(Top); }

void Ovl_Ftrace::Logic() {}

Ovl_UiBattery::Ovl_UiBattery(float *percentage) { _pct_addr = percentage; }

void Ovl_UiBattery::Draw(void) const {}

void Ovl_UiBattery::Logic() {}

Ovl_UiSound::Ovl_UiSound(float *percentage) { _pct_addr = percentage; }

void Ovl_UiSound::Draw(void) const {}

void Ovl_UiSound::Logic() {}

Ovl_Ui3d::Ovl_Ui3d(float *percentage) { _pct_addr = percentage; }

void Ovl_Ui3d::Draw(void) const {}

void Ovl_Ui3d::Logic() {}

Ovl_UiWifi::Ovl_UiWifi(uint8_t *level) { _pct_addr = level; }

void Ovl_UiWifi::Draw(void) const {}

void Ovl_UiWifi::Logic() {}
} // namespace RenderD7