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

#include <string>

namespace RenderD7 {
void Error(const std::string& msg);
inline void InlineError(const std::string& msg) {
  std::string location = __FILE__ + std::string(":") + std::to_string(__LINE__);
  Error("Error: \n" + location + "\n" + msg);
}
inline void InlineAssert(bool v, const std::string& msg) {
  if (v == false) {
    std::string location =
        __FILE__ + std::string(":") + std::to_string(__LINE__);
    Error("Assert Failed:\n" + location + "\n" + msg);
  }
}
}  // namespace RenderD7