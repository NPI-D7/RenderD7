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

#include <filesystem>
#include <fstream>
#include <memory>
#include <renderd7/Logger.hpp>
#include <renderd7/Time.hpp>
#include <renderd7/renderd7.hpp>

namespace RenderD7 {
LoggerBase::~LoggerBase() {
  if (_log.is_open()) _log.close();
}
void LoggerBase::Init(const std::string& name, bool fileless) {
  if (!fileless) {
    std::string path_base = RenderD7::GetAppDirectory() + "/logs/";
    if (!std::filesystem::is_directory(path_base)) {
      std::filesystem::create_directories(path_base);
    }
    auto ts = RenderD7::GetTimeStr();
    std::string fn = name + ts + ".txt";
    this->filename = name;
    this->log_path = path_base + name;
    if (std::filesystem::exists(this->log_path)) {
      // Do nothing
    } else {
      _log.open(this->log_path, std::ios::out);
    }
  }
  this->Write("RenderD7 Log\n\n");
}

void LoggerBase::Write(const std::string& debug_text, int lvl) {
  std::string msg = "[" + RenderD7::GetTimeStr() + "]: " + debug_text;
  if (this->_log.is_open() && lvl <= writelvl) {
    this->_log << msg << std::endl;
  }
  while (msg.find_first_of('\n') != 0) {
    lines.push_back(msg.substr(0, msg.find_first_of('\n')));
    msg = msg.substr(msg.find_first_of('\n'));
  }
  lines.push_back(msg);
}

const std::vector<std::string>& LoggerBase::Lines() { return this->lines; }
}  // namespace RenderD7