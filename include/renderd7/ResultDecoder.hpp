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
#include <3ds.h>

#include <string>

namespace RenderD7 {
/// @brief Decoder for 3ds Result Codes
class ResultDecoder {
 public:
  /// @brief Constructor
  ResultDecoder() {}
  /// @brief Deconstructor
  ~ResultDecoder() {}
  /// @brief Load a Result into Decoder
  /// @param rescode Result Code
  void Load(Result rescode);
  /// @brief Load A Hex Converted Code into Decoder
  /// @param rescode Result-Hex Code
  void Load(std::string rescode);
  /// @brief Get Hex Code
  /// @return Hex-Code
  std::string GetCode();
  /// @brief Get Level Name
  /// @return Level Name
  std::string GetLevel();
  /// @brief Get Level Value
  /// @return Level Value
  int GetLevelInt();
  /// @brief Get The Mosule Name
  /// @return Module Name
  std::string GetModule();
  /// @brief Get The Module Value
  /// @return Module Value
  int GetModuleInt();
  /// @brief Get The Description
  /// @return Description
  std::string GetDescription();
  /// @brief Get The Description Valur
  /// @return Description Value
  int GetDescriptionInt();
  /// @brief Get the Summary
  /// @return Summary
  std::string GetSummary();
  /// @brief Get the Summary Value
  /// @return Summary Value
  int GetSummaryInt();
  /// @brief Write a Result log file to sd
  void WriteLog(void);

 private:
  /// @param m_rescode Result code
  Result m_rescode;
};
}  // namespace RenderD7