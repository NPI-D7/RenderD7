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
#include <sstream>
#include <string>

namespace RenderD7 {
/// @brief StealConsole Class
class StealConsole {
 public:
  /// @brief Constructor
  StealConsole();
  /// @brief Deconstructor
  ~StealConsole();
  /// @brief The Stolen Stdout
  /// @return Stdout as string
  std::string GetStdout();

 private:
  /// @param stolen_stdout Stolen Stdout
  std::stringstream stolen_stdout;
};
}  // namespace RenderD7