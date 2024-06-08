#include <renderd7/Render2.hpp>

namespace RenderD7 {
R2Base::R2Base() {
  for (int i = 0; i < 2; i++) this->font[i] = Font::New();
}
}  // namespace RenderD7