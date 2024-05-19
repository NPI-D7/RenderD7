/**
 *   This file is part of RenderD7
 *   Copyright (C) 2021-2024 NPI-D7, tobid7
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "scene.hpp"

extern void IdbServer();

int main() {
  rd7_enable_memtrack = true;
  rd7_do_splash = true;
  RD7::Ftrace::Beg("app", "app_init");
  RD7::Init::Main("rd7tf");
  RD7::FadeIn();
  auto fnt = RD7::Font::New("romfs:/roboto_bold.bcfnt");
  RD7::TextFont(fnt);
  // IdbServer();
  // RD7::Init::NdspFirm();
  RD7::Scene::Load(std::make_unique<Sample>());
  RD7::Ftrace::End("app", "app_init");
  while (RD7::MainLoop()) {
    RD7::OnScreen(Top);
    RD7::Ftrace::Beg("app", "app_mainloop");
    if (d7_hDown & KEY_START) {
      RD7::FadeOut();
      RD7::ExitApp();
    }
    RD7::OnScreen(Top);
    RD7::FrameEnd();
    RD7::Ftrace::End("app", "app_mainloop");
  }
  return 0;
}