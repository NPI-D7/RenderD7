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

#include <map>
#include <renderd7/Color.hpp>
#include <renderd7/Font.hpp>
#include <renderd7/R7Vec.hpp>
#include <renderd7/smart_ctor.hpp>

namespace RenderD7 {
class R2Base {
 public:
  R2Base();
  ~R2Base() = default;
  void SetFont();
  Font::Ref GetFont();
  void DefaultFont();

 private:
  const float default_text_size = 0.5f;
  float text_size = 0.5;
  Font::Ref font[2];
  R7Vec2 max_wh;
  std::map<std::string, float> ts;
  std::map<std::string, int> mln;
};
}  // namespace RenderD7