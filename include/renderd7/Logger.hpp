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