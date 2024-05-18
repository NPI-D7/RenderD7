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
#include <renderd7/log.hpp>

std::string Log::format(const std::string &fmt_str, ...) {
  va_list ap;
  char *fp = NULL;
  va_start(ap, fmt_str);
  vasprintf(&fp, fmt_str.c_str(), ap);
  va_end(ap);
  std::unique_ptr<char, decltype(free) *> formatted(fp, free);
  return std::string(formatted.get());
}

std::string Log::logDate(void) {
  time_t unixTime;
  struct tm timeStruct;
  time(&unixTime);
  localtime_r(&unixTime, &timeStruct);
  return format("%04i-%02i-%02i_%02i-%02i-%02i", timeStruct.tm_year + 1900,
                timeStruct.tm_mon + 1, timeStruct.tm_mday, timeStruct.tm_hour,
                timeStruct.tm_min, timeStruct.tm_sec);
}

Log::Log() {}

void Log::Init(const char *filename) {
  printf("%s\n", filename);
  std::string fn = filename;
  std::string name = fn + ".txt";
  this->filename = name;
  if (std::filesystem::exists(name)) {
    // Do nothing
  } else {
    std::fstream f(name, std::ios::out);
    f.close();
  }
}

void Log::Write(std::string debug_text) {
  printf("%s\n", debug_text.c_str());
  std::ofstream logFile;
  logFile.open((this->filename), std::ofstream::app);
  std::string writeDebug = "[";
  writeDebug += logDate();
  writeDebug += "] ";
  writeDebug += debug_text.c_str();
  logFile << writeDebug << std::endl;
  logFile.close();
}
Log::~Log() {}
