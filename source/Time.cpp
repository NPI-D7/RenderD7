#include <fstream>
#include <memory>
#include <renderd7/Time.hpp>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <unistd.h>

std::string RenderD7::FormatString(std::string fmt_str, ...) {
  va_list ap;
  char *fp = NULL;
  va_start(ap, fmt_str);
  vasprintf(&fp, fmt_str.c_str(), ap);
  va_end(ap);
  std::unique_ptr<char, decltype(free) *> formatted(fp, free);
  return std::string(formatted.get());
}

std::string RenderD7::GetTimeStr(void) {
  time_t unixTime;
  struct tm timeStruct;
  time(&unixTime);
  localtime_r(&unixTime, &timeStruct);
  return FormatString("%04i-%02i-%02i_%02i-%02i-%02i",
                      timeStruct.tm_year + 1900, timeStruct.tm_mon + 1,
                      timeStruct.tm_mday, timeStruct.tm_hour, timeStruct.tm_min,
                      timeStruct.tm_sec);
}
