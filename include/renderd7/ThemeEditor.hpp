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

#pragma once

#include <renderd7/renderd7.hpp>

namespace RenderD7 {
class ThemeEditor : public RenderD7::Scene {
 public:
  ThemeEditor();
  ~ThemeEditor();

  void Draw(void) const override;
  void Logic() override;

 private:
  Theme::Ref edit_theme;
  // Placeholder to save active one to
  Theme::Ref temp_theme;

  // temp vars for samples
  mutable bool cm;
  mutable std::string inpt;
  mutable int menu = 0;
};
}  // namespace RenderD7