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
#include <fstream>
#include <renderd7/smart_ctor.hpp>
#include <string>
#include <vector>

namespace RenderD7 {

/// @brief Logger base Class
class LoggerBase {
 public:
  /// @brief Constructor
  LoggerBase() = default;
  /// @brief Deconstructor
  ~LoggerBase();
  RD7_SMART_CTOR(LoggerBase)
  /// @brief Init the Logger
  /// @param filename name[_date_time.txt]
  void Init(const std::string& name, bool fileless = false);
  /// @brief Write a String
  /// @param debug_text string
  /// @param lvl Logger LVL 0 = ERR, 1  =WARNING, >=2= Default
  void Write(const std::string& debug_text, int lvl = 2);
  void SetLvl(int lvl) { writelvl = lvl; }
  const std::vector<std::string>& Lines();

 private:
  /// \param filename the name of the logfile
  std::string filename;
  std::string log_path;
  std::ofstream _log;
  int writelvl = 1;  // Only log errors/Warnings
  std::vector<std::string> lines;
};
}  // namespace RenderD7