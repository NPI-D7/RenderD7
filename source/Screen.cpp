#include <renderd7/Screen.hpp>
#include <renderd7/internal_db.hpp>

void RenderD7::OnScreen(C3D_RenderTarget *target) {
  C2D_SceneBegin(target);
  rd7i_current_screen = (target == Top || target == TopRight) ? 1 : 0;
}