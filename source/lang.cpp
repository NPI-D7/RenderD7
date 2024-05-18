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

#include <3ds.h>

#include <filesystem>
#include <fstream>
#include <renderd7/lang.hpp>

static nlohmann::json appJson;

std::string RenderD7::Lang::GetSys() {
  u8 language = 1;
  CFGU_GetSystemLanguage(&language);

  switch (language) {
    case 0:
      return "jp";  // Japanese
      break;

    case 1:
      return "en";  // English
      break;

    case 2:
      return "fr";  // French
      break;

    case 3:
      return "de";  // German
      break;

    case 4:
      return "it";  // Italian
      break;

    case 5:
      return "es";  // Spanish
      break;

    case 6:
      return "zh-CN";  // Chinese (Simplified)
      break;

    case 7:
      return "ko";  // Korean
      break;

    case 8:
      return "nl";  // Dutch
      break;

    case 9:
      return "pt";  // Portuguese
      break;

    case 10:
      return "ru";  // Russian
      break;

    case 11:
      return "zh-TW";  // Chinese (Traditional)
      break;

    default:
      return "en";  // Fall back to English if missing
      break;
  }
}
std::string RenderD7::Lang::Get(const std::string &key) {
  if (!appJson.contains("keys")) return "ERR-01";
  nlohmann::json js = appJson["keys"];
  if (!js.contains(key)) return key;
  return js.at(key).get<std::string>();
}

void RenderD7::Lang::Load(const std::string &lang) {
  std::fstream values;
  if (std::filesystem::exists("romfs:/lang/" + lang + "/app.json")) {
    values.open("romfs:/lang/" + lang + "/app.json", std::ios::in);
    if (values.is_open()) {
      appJson = nlohmann::json::parse(values);
    }
    values.close();
    if (appJson.is_discarded()) {
      appJson = {};
    }
    return;
  } else {
    values.open("romfs:/lang/en/app.json", std::ios::in);
    if (values.is_open()) {
      appJson = nlohmann::json::parse(values);
    }
    values.close();
    if (appJson.is_discarded()) {
      appJson = {};
    }
    return;
  }
}

std::string RenderD7::Lang::GetName() {
  if (!appJson.contains("info")) return "";
  nlohmann::json js = appJson["info"];
  if (!js.contains("name")) return "Unknown";
  return js.at("name").get<std::string>();
}

std::string RenderD7::Lang::GetAuthor() {
  if (!appJson.contains("info")) return "";
  nlohmann::json js = appJson["info"];
  if (!js.contains("author")) return "Unknown";
  return js.at("author").get<std::string>();
}

std::string RenderD7::Lang::GetShortcut() {
  if (!appJson.contains("info")) return "";
  nlohmann::json js = appJson["info"];
  if (!js.contains("shortcut")) return "Unknown";
  return js.at("shortcut").get<std::string>();
}