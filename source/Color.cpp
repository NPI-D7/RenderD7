#include <filesystem>
#include <fstream>
#include <map>
#include <renderd7/Color.hpp>
#include <renderd7/external/json.hpp>
#include <renderd7/internal_db.hpp>

// This is btw the default theme setup
// RenderD7 StyleColor Api
// not const cause const = error lol
std::map<RD7Color, unsigned int> rd7i_color_map = {
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

std::map<RD7Color, unsigned int> rd7i_color_swap_map;

unsigned int RenderD7::StyleColor(RD7Color color) {
  if (rd7i_color_swap_map.find(color) != rd7i_color_swap_map.end())
    return rd7i_color_swap_map[color];
  else if (rd7i_color_map.find(color) != rd7i_color_map.end())
    return rd7i_color_map[color];
  return RGBA8(0, 0, 0, 0);
}

void RenderD7::ColorNew(RD7Color color, unsigned int new_color) {
  // Dont edit before Init
  if (!rd7i_running) return;
  rd7i_color_map[color] = new_color;
}

void RenderD7::RedirectColor(RD7Color to, RD7Color from) {
  // As you see at the code Redirect doesnt redirect xd
  // Just named cause it feels like redirecting
  // Oh and if the color is edit you redirect to it is
  // grabs that redirected lol
  rd7i_color_swap_map[to] = StyleColor(from);
}

void RenderD7::CustomizeColor(RD7Color color, unsigned int custom) {
  rd7i_color_swap_map[color] = custom;
}

void RenderD7::TextColorByBg(RD7Color background) {
  UndoColorEdit(RD7Color_Text);  // To be sure
  rd7i_color_swap_map[RD7Color_Text] = StyleColor(
      Color::RGBA(background).is_light() ? RD7Color_Text : RD7Color_Text2);
}

void RenderD7::UndoColorEdit(RD7Color color) {
  if (rd7i_color_swap_map.find(color) == rd7i_color_swap_map.end()) return;
  rd7i_color_swap_map.erase(color);
}

void RenderD7::UndoAllColorEdits() { rd7i_color_swap_map.clear(); }

void rd7i_swap32(unsigned int& c) {
  c = ((c & 0xFF) << 24) | ((c & 0xFF00) << 8) | ((c & 0xFF0000) >> 8) |
      ((c & 0xFF000000) >> 24);
}

std::string rd7i_mk2hex(unsigned int c32) {
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

void RenderD7::ThemeLoad(const std::string& path) {
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
  rd7i_color_map[RD7Color_Text] =rd7i_special_color_hex(js["RD7Color_Text"].get<std::string>());
  rd7i_color_map[RD7Color_Text2] =rd7i_special_color_hex(js["RD7Color_Text2"].get<std::string>());
  rd7i_color_map[RD7Color_TextDisabled] =rd7i_special_color_hex(js["RD7Color_TextDisabled"].get<std::string>());
  rd7i_color_map[RD7Color_Background] =rd7i_special_color_hex(js["RD7Color_Background"].get<std::string>());
  rd7i_color_map[RD7Color_Header] =rd7i_special_color_hex(js["RD7Color_Header"].get<std::string>());
  rd7i_color_map[RD7Color_Selector] =rd7i_special_color_hex(js["RD7Color_Selector"].get<std::string>());
  rd7i_color_map[RD7Color_SelectorFade] =rd7i_special_color_hex(js["RD7Color_SelectorFade"].get<std::string>());
  rd7i_color_map[RD7Color_List0] =rd7i_special_color_hex(js["RD7Color_List0"].get<std::string>());
  rd7i_color_map[RD7Color_List1] =rd7i_special_color_hex(js["RD7Color_List1"].get<std::string>());
  rd7i_color_map[RD7Color_MessageBackground] = rd7i_special_color_hex(js["RD7Color_MessageBackground"].get<std::string>());
  rd7i_color_map[RD7Color_Button] =rd7i_special_color_hex(js["RD7Color_Button"].get<std::string>());
  rd7i_color_map[RD7Color_ButtonHovered] =rd7i_special_color_hex(js["RD7Color_ButtonHovered"].get<std::string>());
  rd7i_color_map[RD7Color_ButtonDisabled] =rd7i_special_color_hex(js["RD7Color_ButtonDisabled"].get<std::string>());
  rd7i_color_map[RD7Color_ButtonActive] =rd7i_special_color_hex(js["RD7Color_ButtonActive"].get<std::string>());
  rd7i_color_map[RD7Color_Checkmark] =rd7i_special_color_hex(js["RD7Color_Checkmark"].get<std::string>());
  rd7i_color_map[RD7Color_FrameBg] =rd7i_special_color_hex(js["RD7Color_FrameBg"].get<std::string>());
  rd7i_color_map[RD7Color_FrameBgHovered] =rd7i_special_color_hex(js["RD7Color_FrameBgHovered"].get<std::string>());
  rd7i_color_map[RD7Color_Progressbar] =rd7i_special_color_hex(js["RD7Color_Progressbar"].get<std::string>());
  // clang-format on
  file.close();
}

void RenderD7::ThemeSave(const std::string& path) {
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
  js["RD7Color_Text"] = rd7i_mk2hex(rd7i_color_map[RD7Color_Text]);
  js["RD7Color_Text2"] = rd7i_mk2hex(rd7i_color_map[RD7Color_Text2]);
  js["RD7Color_TextDisabled"] = rd7i_mk2hex(rd7i_color_map[RD7Color_TextDisabled]);
  js["RD7Color_Background"] = rd7i_mk2hex(rd7i_color_map[RD7Color_Background]);
  js["RD7Color_Header"] = rd7i_mk2hex(rd7i_color_map[RD7Color_Header]);
  js["RD7Color_Selector"] = rd7i_mk2hex(rd7i_color_map[RD7Color_Selector]);
  js["RD7Color_SelectorFade"] = rd7i_mk2hex(rd7i_color_map[RD7Color_SelectorFade]);
  js["RD7Color_List0"] = rd7i_mk2hex(rd7i_color_map[RD7Color_List0]);
  js["RD7Color_List1"] = rd7i_mk2hex(rd7i_color_map[RD7Color_List1]);
  js["RD7Color_MessageBackground"] = rd7i_mk2hex(rd7i_color_map[RD7Color_MessageBackground]);
  js["RD7Color_Button"] = rd7i_mk2hex(rd7i_color_map[RD7Color_Button]);
  js["RD7Color_ButtonHovered"] = rd7i_mk2hex(rd7i_color_map[RD7Color_ButtonHovered]);
  js["RD7Color_ButtonDisabled"] = rd7i_mk2hex(rd7i_color_map[RD7Color_ButtonDisabled]);
  js["RD7Color_ButtonActive"] = rd7i_mk2hex(rd7i_color_map[RD7Color_ButtonActive]);
  js["RD7Color_Checkmark"] = rd7i_mk2hex(rd7i_color_map[RD7Color_Checkmark]);
  js["RD7Color_FrameBg"] = rd7i_mk2hex(rd7i_color_map[RD7Color_FrameBg]);
  js["RD7Color_FrameBgHovered"] = rd7i_mk2hex(rd7i_color_map[RD7Color_FrameBgHovered]);
  js["RD7Color_Progressbar"] = rd7i_mk2hex(rd7i_color_map[RD7Color_Progressbar]);
  // clang-format on
  file << js.dump(4);
  file.close();
}

void RenderD7::ThemeDefault() {
  rd7i_color_map[RD7Color_Text] = RGBA8(0, 0, 0, 255);
  rd7i_color_map[RD7Color_Text2] = RGBA8(255, 255, 255, 255);
  rd7i_color_map[RD7Color_TextDisabled] = RGBA8(170, 170, 170, 255);
  rd7i_color_map[RD7Color_Background] = RGBA8(238, 238, 238, 255);
  rd7i_color_map[RD7Color_Header] = RGBA8(17, 17, 17, 255);
  rd7i_color_map[RD7Color_Selector] = RGBA8(34, 34, 34, 255);
  rd7i_color_map[RD7Color_SelectorFade] = RGBA8(90, 90, 90, 255);
  rd7i_color_map[RD7Color_List0] = RGBA8(204, 204, 204, 255);
  rd7i_color_map[RD7Color_List1] = RGBA8(187, 187, 187, 255);
  rd7i_color_map[RD7Color_MessageBackground] = RGBA8(51, 51, 51, 255);
  rd7i_color_map[RD7Color_Button] = RGBA8(17, 17, 17, 255);
  rd7i_color_map[RD7Color_ButtonHovered] = RGBA8(34, 34, 34, 255);
  rd7i_color_map[RD7Color_ButtonDisabled] = RGBA8(8, 8, 8, 255);
  rd7i_color_map[RD7Color_ButtonActive] = RGBA8(42, 42, 42, 255);
  rd7i_color_map[RD7Color_Checkmark] = RGBA8(42, 42, 42, 255);
  rd7i_color_map[RD7Color_FrameBg] = RGBA8(85, 85, 85, 255);
  rd7i_color_map[RD7Color_FrameBgHovered] = RGBA8(119, 119, 119, 255);
  rd7i_color_map[RD7Color_Progressbar] = RGBA8(0, 255, 0, 255);
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