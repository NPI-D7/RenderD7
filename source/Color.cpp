#include <map>
#include <renderd7/Color.hpp>

#define RGBA8(r, g, b, a)                                                      \
  ((((r)&0xFF) << 0) | (((g)&0xFF) << 8) | (((b)&0xFF) << 16) |                \
   (((a)&0xFF) << 24))

// uint32_t RenderD7::Color::Hex(const std::string &color, uint8_t a) {
//   if (color.length() < 7 ||
//       std::regex_search(color.substr(1),
//                         std::regex("[^0-9A-Fa-f]"))) { // invalid color.
//     return RenderD7::Color::Hex("#000000", 0);
//   }
//   int r = std::stoi(color.substr(1, 2), nullptr, 16);
//   int g = std::stoi(color.substr(3, 2), nullptr, 16);
//   int b = std::stoi(color.substr(5, 2), nullptr, 16);
//   return RGBA8(r, g, b, a);
// }

// Lookup-Table für Hex-to-Dez-Konvertierung
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