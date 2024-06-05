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
#include <map>
#include <renderd7/Color.hpp>
#include <renderd7/external/json.hpp>
#include <renderd7/internal_db.hpp>

void rd7i_swap32(unsigned int& c) {
  c = ((c & 0xFF) << 24) | ((c & 0xFF00) << 8) | ((c & 0xFF0000) >> 8) |
      ((c & 0xFF000000) >> 24);
}

std::string RenderD7::Color::RGBA2Hex(unsigned int c32) {
  rd7i_swap32(c32);
  std::stringstream ss;
  ss << "#";
  ss << std::hex << std::setw(8) << std::setfill('0') << c32;
  return ss.str();
}

// Standart Color Converter
static const std::map<char, int> HEX_TO_DEC = {
    {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
    {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'a', 10}, {'b', 11},
    {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}};

unsigned int rd7i_special_color_hex(const std::string& hex) {
  if (hex.length() < 9 || std::find_if(hex.begin() + 1, hex.end(), [](char c) {
                            return !std::isxdigit(c);
                          }) != hex.end()) {
    return rd7i_special_color_hex("#00000000");
  }

  int r = HEX_TO_DEC.at(hex[1]) * 16 + HEX_TO_DEC.at(hex[2]);
  int g = HEX_TO_DEC.at(hex[3]) * 16 + HEX_TO_DEC.at(hex[4]);
  int b = HEX_TO_DEC.at(hex[5]) * 16 + HEX_TO_DEC.at(hex[6]);
  int a = HEX_TO_DEC.at(hex[7]) * 16 + HEX_TO_DEC.at(hex[8]);

  return RGBA8(r, g, b, a);
}

// Default Theme
const std::map<RD7Color, unsigned int> rd7i_default_theme = {
    {RD7Color_Text, RGBA8(0, 0, 0, 255)},
    {RD7Color_Text2, RGBA8(255, 255, 255, 255)},  // For Background change or so
    {RD7Color_TextDisabled, RGBA8(170, 170, 170, 255)},
    {RD7Color_Background, RGBA8(238, 238, 238, 255)},
    {RD7Color_Header, RGBA8(17, 17, 17, 255)},
    {RD7Color_Selector, RGBA8(34, 34, 34, 255)},
    {RD7Color_SelectorFade, RGBA8(90, 90, 90, 255)},
    {RD7Color_List0, RGBA8(204, 204, 204, 255)},  // List0 = % 2
    {RD7Color_List1, RGBA8(187, 187, 187, 255)},
    {RD7Color_MessageBackground, RGBA8(51, 51, 51, 255)},
    {RD7Color_Button, RGBA8(17, 17, 17, 255)},
    {RD7Color_ButtonHovered, RGBA8(34, 34, 34, 255)},
    {RD7Color_ButtonDisabled, RGBA8(8, 8, 8, 255)},
    {RD7Color_ButtonActive, RGBA8(42, 42, 42, 255)},
    {RD7Color_Checkmark, RGBA8(42, 42, 42, 255)},
    {RD7Color_FrameBg, RGBA8(85, 85, 85, 255)},
    {RD7Color_FrameBgHovered, RGBA8(119, 119, 119, 255)},
    {RD7Color_Progressbar, RGBA8(0, 255, 0, 255)},
};

void RenderD7::Theme::Load(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return;
  }
  nlohmann::json js;
  file >> js;
  // clang-format off
  if(THEMEVER != js["version"]) {
    file.close();
    return; 
  }
  this->clr_tab.clear();
  this->clr_tab.resize(RD7Color_Len);
  this->clr_tab[RD7Color_Text] = rd7i_special_color_hex(js["RD7Color_Text"].get<std::string>());
  this->clr_tab[RD7Color_Text2] = rd7i_special_color_hex(js["RD7Color_Text2"].get<std::string>());
  this->clr_tab[RD7Color_TextDisabled] = rd7i_special_color_hex(js["RD7Color_TextDisabled"].get<std::string>());
  this->clr_tab[RD7Color_Background] = rd7i_special_color_hex(js["RD7Color_Background"].get<std::string>());
  this->clr_tab[RD7Color_Header] = rd7i_special_color_hex(js["RD7Color_Header"].get<std::string>());
  this->clr_tab[RD7Color_Selector] = rd7i_special_color_hex(js["RD7Color_Selector"].get<std::string>());
  this->clr_tab[RD7Color_SelectorFade] = rd7i_special_color_hex(js["RD7Color_SelectorFade"].get<std::string>());
  this->clr_tab[RD7Color_List0] = rd7i_special_color_hex(js["RD7Color_List0"].get<std::string>());
  this->clr_tab[RD7Color_List1] = rd7i_special_color_hex(js["RD7Color_List1"].get<std::string>());
  this->clr_tab[RD7Color_MessageBackground] =  rd7i_special_color_hex(js["RD7Color_MessageBackground"].get<std::string>());
  this->clr_tab[RD7Color_Button] = rd7i_special_color_hex(js["RD7Color_Button"].get<std::string>());
  this->clr_tab[RD7Color_ButtonHovered] = rd7i_special_color_hex(js["RD7Color_ButtonHovered"].get<std::string>());
  this->clr_tab[RD7Color_ButtonDisabled] = rd7i_special_color_hex(js["RD7Color_ButtonDisabled"].get<std::string>());
  this->clr_tab[RD7Color_ButtonActive] = rd7i_special_color_hex(js["RD7Color_ButtonActive"].get<std::string>());
  this->clr_tab[RD7Color_Checkmark] = rd7i_special_color_hex(js["RD7Color_Checkmark"].get<std::string>());
  this->clr_tab[RD7Color_FrameBg] = rd7i_special_color_hex(js["RD7Color_FrameBg"].get<std::string>());
  this->clr_tab[RD7Color_FrameBgHovered] = rd7i_special_color_hex(js["RD7Color_FrameBgHovered"].get<std::string>());
  this->clr_tab[RD7Color_Progressbar] = rd7i_special_color_hex(js["RD7Color_Progressbar"].get<std::string>());
  // clang-format on
  file.close();
}

void RenderD7::Theme::Default() {
  this->clr_tab.clear();
  this->clr_tab.resize(RD7Color_Len);
  for (auto& it : rd7i_default_theme) {
    this->clr_tab[it.first] = it.second;
  }
}

unsigned int RenderD7::Theme::Get(RD7Color clr) {
  if (clr < 0 || clr >= RD7Color_Len) return 0;
  return this->clr_tab[clr];
}

void RenderD7::Theme::Set(RD7Color clr, unsigned int v) {
  if (clr < 0 || clr >= RD7Color_Len) return;
  this->changes.push_back(change(clr, this->clr_tab[clr], v));
  this->clr_tab[clr] = v;
}
void RenderD7::Theme::Swap(RD7Color a, RD7Color b) {
  if (a < 0 || a >= RD7Color_Len || b < 0 || b >= RD7Color_Len) return;
  auto c = this->clr_tab[a];
  this->clr_tab[a] = this->clr_tab[b];
  this->clr_tab[b] = c;
  this->changes.push_back(change(a, b, c, this->clr_tab[a]));
}

void RenderD7::Theme::TextBy(RD7Color bg) {
  if (!Color::RGBA(bg).is_light()) Swap(RD7Color_Text, RD7Color_Text2);
}

RD7Color RenderD7::Theme::AutoText(RD7Color bg) {
  return Color::RGBA(bg).is_light() ? RD7Color_Text : RD7Color_Text2;
}

bool RenderD7::Theme::Undo() {
  if (!this->changes.size()) return false;
  auto ch = this->changes[this->changes.size() - 1];
  this->changes.pop_back();
  if (ch.clr2) {
    this->clr_tab[ch.clr2] = ch.to;
    this->clr_tab[ch.clr] = ch.from;
  } else {
    this->clr_tab[ch.clr] = ch.from;
  }
  return true;
}

void RenderD7::Theme::UndoAll() {
  while (Undo()) {
    // Just Run Undo Until all is undone
  }
}

void RenderD7::Theme::Save(const std::string& path) {
  if (std::filesystem::path(path).filename().string() == "renderd7.theme") {
    if (!rd7i_amdt) {
      return;
    }
  }
  std::ofstream file(path);
  if (!file.is_open()) {
    return;
  }
  nlohmann::json js;
  // clang-format off
  js["version"] = THEMEVER;
  js["RD7Color_Text"] = RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_Text]);
  js["RD7Color_Text2"] = RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_Text2]);
  js["RD7Color_TextDisabled"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_TextDisabled]); js["RD7Color_Background"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_Background]); js["RD7Color_Header"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_Header]); js["RD7Color_Selector"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_Selector]); js["RD7Color_SelectorFade"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_SelectorFade]); js["RD7Color_List0"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_List0]); js["RD7Color_List1"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_List1]); js["RD7Color_MessageBackground"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_MessageBackground]); js["RD7Color_Button"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_Button]); js["RD7Color_ButtonHovered"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_ButtonHovered]);
  js["RD7Color_ButtonDisabled"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_ButtonDisabled]);
  js["RD7Color_ButtonActive"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_ButtonActive]); js["RD7Color_Checkmark"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_Checkmark]); js["RD7Color_FrameBg"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_FrameBg]); js["RD7Color_FrameBgHovered"] =
RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_FrameBgHovered]); js["RD7Color_Progressbar"]
= RenderD7::Color::RGBA2Hex(this->clr_tab[RD7Color_Progressbar]);
  // clang-format on
  file << js.dump(4);
  file.close();
}

RenderD7::Theme::Ref RenderD7::ThemeActive() { return rd7i_active_theme; }

void RenderD7::ThemeSet(RenderD7::Theme::Ref theme) {
  rd7i_active_theme = theme;
}

uint32_t RenderD7::Color::Hex(const std::string& color, uint8_t a) {
  if (color.length() < 7 ||
      std::find_if(color.begin() + 1, color.end(),
                   [](char c) { return !std::isxdigit(c); }) != color.end()) {
    return RenderD7::Color::Hex("#000000", 0);
  }

  int r = HEX_TO_DEC.at(color[1]) * 16 + HEX_TO_DEC.at(color[2]);
  int g = HEX_TO_DEC.at(color[3]) * 16 + HEX_TO_DEC.at(color[4]);
  int b = HEX_TO_DEC.at(color[5]) * 16 + HEX_TO_DEC.at(color[6]);

  return RGBA8(r, g, b, a);
}

std::string RenderD7::Color::RGB2Hex(int r, int g, int b) {
  std::stringstream ss;
  ss << "#";
  ss << std::hex << (r << 16 | g << 8 | b);
  return ss.str();
}