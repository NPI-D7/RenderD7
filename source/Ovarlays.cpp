#include <renderd7/FunctionTrace.hpp>
#include <renderd7/Ovarlays.hpp>
#include <renderd7/renderd7.hpp>

namespace RenderD7 {
Ovl_Ftrace::Ovl_Ftrace() {}

void Ovl_Ftrace::Draw(void) const { RenderD7::OnScreen(Top); }

void Ovl_Ftrace::Logic() {}
} // namespace RenderD7