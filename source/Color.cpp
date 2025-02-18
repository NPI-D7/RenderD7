#include <map>
#include <renderd7/Color.hpp>

// This is btw the default theme setup
// RenderD7 StyleColor Api
// not const cause const = error lol
std::map<RD7Color, unsigned int> rd7i_color_map = {
    {RD7Color_Text, RGBA8(0, 0, 0, 255)},
    {RD7Color_Text2, RGBA8(255, 255, 255, 255)}, // For Background change or so
    {RD7Color_TextDisabled, RGBA8(170, 170, 170, 255)},
    {RD7Color_Background, RGBA8(238, 238, 238, 255)},
    {RD7Color_Header, RGBA8(17, 17, 17, 255)},
    {RD7Color_Selector, RGBA8(34, 34, 34, 255)},
    {RD7Color_SelectorFade, RGBA8(90, 90, 90, 255)},
    {RD7Color_List0, RGBA8(204, 204, 204, 255)}, // List0 = % 2
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
  UndoColorEdit(RD7Color_Text); // To be sure
  rd7i_color_swap_map[RD7Color_Text] = StyleColor(
      Color::RGBA(background).is_light() ? RD7Color_Text : RD7Color_Text2);
}

void RenderD7::UndoColorEdit(RD7Color color) {
  if (rd7i_color_swap_map.find(color) == rd7i_color_swap_map.end())
    return;
  rd7i_color_swap_map.erase(color);
}

void RenderD7::UndoAllColorEdits() { rd7i_color_swap_map.clear(); }

// Standart Color Converter
static const std::map<char, int> HEX_TO_DEC = {
    {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
    {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'a', 10}, {'b', 11},
    {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}};

uint32_t RenderD7::Color::Hex(const std::string &color, uint8_t a) {
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