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
// Base includes
#include <map>
#include <string>
// 3ds does not support std::chrono
#include <3ds.h>

/// @brief 3ds System Ticks per milli second
#define TICKS_PER_MSEC 268111.856

#define f2s(x_) #x_
#define scomb(x1, x2) std::string(x1 + x2)

namespace RenderD7 {
namespace Ftrace {
/// @brief Result of FTrace
struct FTRes {
  std::string group;      ///< Group of the Trace
  std::string func_name;  ///< Function Name

  uint64_t time_start;  ///< when started
  uint64_t time_end;    ///< when stopped
  float time_of;        ///< stop - start (how long)
  bool is_ovl;          ///< is displayed in overlay?
};

/// @brief Map of Traces
extern std::map<std::string, RenderD7::Ftrace::FTRes> rd7_traces;

/// @brief Set a Start TracePoint
/// @param group Set a Group Name
/// @param func_name Set a Function Name
inline void Beg(std::string group, std::string func_name) {
  std::string trace_id = scomb(group, func_name);
  rd7_traces[trace_id].group = group;
  rd7_traces[trace_id].func_name = func_name;
  rd7_traces[trace_id].time_start = svcGetSystemTick();
}
/// @brief Set an End TracePoint
/// @param group Set a Group Name
/// @param func_name Set a Function Name
inline void End(std::string group, std::string func_name) {
  std::string trace_id = scomb(group, func_name);
  rd7_traces[trace_id].time_end = svcGetSystemTick();
  rd7_traces[trace_id].time_of = static_cast<float>(
      ((float)rd7_traces[trace_id].time_end / (float)TICKS_PER_MSEC) -
      ((float)rd7_traces[trace_id].time_start / (float)TICKS_PER_MSEC));
}
}  // namespace Ftrace
}  // namespace RenderD7
