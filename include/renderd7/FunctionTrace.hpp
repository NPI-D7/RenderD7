#pragma once
// Base includes
#include <functional>
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
  float time_ofm;       ///< max time off
  bool is_ovl;          ///< is displayed in overlay?
};

/// @brief Map of Traces
extern std::map<std::string, RenderD7::Ftrace::FTRes> rd7_traces;

/// @brief Set a Start TracePoint
/// @param group Set a Group Name
/// @param func_name Set a Function Name
inline void Beg(const std::string& group, const std::string& func_name) {
  std::string trace_id = scomb(group, func_name);
  auto& trace = rd7_traces[trace_id];
  trace.group = group;
  trace.func_name = func_name;
  trace.time_start = svcGetSystemTick();
}
/// @brief Set an End TracePoint
/// @param group Set a Group Name
/// @param func_name Set a Function Name
inline void End(const std::string& group, const std::string& func_name) {
  std::string trace_id = scomb(group, func_name);
  auto& trace = rd7_traces[trace_id];
  trace.time_end = svcGetSystemTick();
  if (trace.time_of > trace.time_ofm) trace.time_ofm = trace.time_of;
  trace.time_of =
      static_cast<float>(trace.time_end - trace.time_start) / TICKS_PER_MSEC;
}
/// @brief Trace a function execution
/// @param group Set a Group Name
/// @param name Set a Function Name
inline void Func(const std::string& group, const std::string& name,
                 std::function<void()> fun) {
  if (!fun) return;
  Beg(group, name);
  fun();
  End(group, name);
}

/// @brief This Starts an Ftrace and
/// end ist when going out of scope
struct ScopedTrace {
  ScopedTrace(std::string g, std::string n) : group(g), name(n) {
    Ftrace::Beg(g, n);
  }
  ~ScopedTrace() { Ftrace::End(group, name); }
  std::string group;
  std::string name;
};
}  // namespace Ftrace
}  // namespace RenderD7
