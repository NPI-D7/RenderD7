#include <renderd7/Color.hpp>

#define RGBA8(r, g, b, a)                                                      \
  ((((r)&0xFF) << 0) | (((g)&0xFF) << 8) | (((b)&0xFF) << 16) |                \
   (((a)&0xFF) << 24))

uint32_t RenderD7::Color::Hex(const std::string color, uint8_t a) {
  if (color.length() < 7 ||
      std::regex_search(color.substr(1),
                        std::regex("[^0-9A-Fa-f]"))) { // invalid color.
    return RenderD7::Color::Hex("#000000", 0);
  }
  int r = std::stoi(color.substr(1, 2), nullptr, 16);
  int g = std::stoi(color.substr(3, 2), nullptr, 16);
  int b = std::stoi(color.substr(5, 2), nullptr, 16);
  return RGBA8(r, g, b, a);
}

std::string RenderD7::Color::RGB2Hex(int r, int g, int b) {
  std::stringstream ss;
  ss << "#";
  ss << std::hex << (r << 16 | g << 8 | b);
  return ss.str();
}