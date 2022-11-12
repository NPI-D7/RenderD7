#include <renderd7/Screen.hpp>

extern bool currentScreen;

void RenderD7::OnScreen(C3D_RenderTarget *target) {
  C2D_SceneBegin(target);
  currentScreen = (target == Top || target == TopRight) ? 1 : 0;
}