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
#include <stdarg.h>
#include <unistd.h>

#include <string>

/// @brief Log Class
class Log {
 public:
  /// @brief Constructor
  Log();
  /// @brief Deconstructor
  ~Log();
  /// @brief Init the Logger
  /// @param filename Filename[_data_time.log]
  void Init(const char *filename);
  /// @brief Write a String to the File
  /// @param debug_text string
  void Write(std::string debug_text);
  /// @brief Get the Date
  /// @return Date as string fmt[data_time]
  std::string logDate(void);
  /// @brief Format a string like sprintf
  /// @param fmt_str the string wich defines the fmt
  /// @param ... Additional Data
  /// @return Formatted String
  std::string format(const std::string &fmt_str, ...);

 private:
  /// \param filename the name of the logfile
  std::string filename;
};
