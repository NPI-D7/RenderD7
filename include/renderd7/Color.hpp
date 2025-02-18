#pragma once
#include <unistd.h>

#include <cstring>
#include <functional>
#include <memory>
#include <regex>
#include <sstream>
#include <string>

#define UNPACK_RGBA(col) (uint8_t)(col >> 24), (col >> 16), (col >> 8), (col)
#define UNPACK_BGRA(col) (uint8_t)(col >> 8), (col >> 16), (col >> 24), (col)
// it is actually not RGBA lol
inline uint32_t RGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
#define ISIMPLEPAK(x, y) (((x) & 0xff) << y)
  return (ISIMPLEPAK(r, 0) | ISIMPLEPAK(g, 8) | ISIMPLEPAK(b, 16) |
          ISIMPLEPAK(a, 24));
}

/*#define RGBA8(r, g, b, a) \
  ((((r)&0xFF) << 0) | (((g)&0xFF) << 8) | (((b)&0xFF) << 16) |                \
   (((a)&0xFF) << 24))*/

typedef int RD7Color;

// MultiColor (Less FunctionNameLen)

struct Color2 {
  unsigned int color0;
  unsigned int color1;
};

struct Color3 {
  unsigned int color0;
  unsigned int color1;
  unsigned int color2;
};

struct Color4 {
  unsigned int color0;
  unsigned int color1;
  unsigned int color2;
  unsigned int color3;
};

enum RD7Color_ {
  RD7Color_Text,  ///< This Color Should always be used for Light Backgrounds
  RD7Color_TextDisabled,  /// Text Disabled Color
  RD7Color_Text2,         ///< And This want for Texts on Dark Backgrounds
  RD7Color_Background,    ///< Your Bg Color
  RD7Color_Header,  ///< Header Color (if the header is dark text2 is used)
  RD7Color_Selector,
  RD7Color_SelectorFade,
  RD7Color_List0,
  RD7Color_List1,
  RD7Color_MessageBackground,
  RD7Color_Button,
  RD7Color_ButtonHovered,
  RD7Color_ButtonDisabled,
  RD7Color_ButtonActive,
  RD7Color_Checkmark,
  RD7Color_FrameBg,
  RD7Color_FrameBgHovered,
  RD7Color_Progressbar,
};

namespace RenderD7 {
unsigned int StyleColor(RD7Color color);
void RedirectColor(RD7Color to, RD7Color from);
void TextColorByBg(RD7Color background);
/// @brief Customices a color until undone
/// For example with RebderD7::Color::Hex
void CustomizeColor(RD7Color color, unsigned int custom);
/// @brief Completly changes a theme color
void ColorNew(RD7Color color, unsigned int new_color);
void UndoColorEdit(RD7Color color);
void UndoAllColorEdits();
namespace Color {
/// @brief RGBA Class
class RGBA {
 public:
  /// @brief Construct
  /// @param r
  /// @param g
  /// @param b
  /// @param a
  RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
      : m_r(r), m_g(g), m_b(b), m_a(a) {}
  /// @brief Construct
  /// @param r
  /// @param g
  /// @param b
  /// @param a
  RGBA(float r, float g, float b, float a = 1.f)
      : m_r(r * 255.f), m_g(g * 255.f), m_b(b * 255.f), m_a(a * 255.f) {}
  RGBA(unsigned int in) {
#define ISIMPLEUNPAK(x, y) (((x) >> y) & 0xFF)
    m_r = ISIMPLEUNPAK(in, 0);
    m_g = ISIMPLEUNPAK(in, 8);
    m_b = ISIMPLEUNPAK(in, 16);
    m_a = ISIMPLEUNPAK(in, 24);
  }
  RGBA(RD7Color in) {
    unsigned int col = RenderD7::StyleColor(in);
    m_r = ISIMPLEUNPAK(col, 0);
    m_g = ISIMPLEUNPAK(col, 8);
    m_b = ISIMPLEUNPAK(col, 16);
    m_a = ISIMPLEUNPAK(col, 24);
  }
  RGBA &changeR(unsigned char r) {
    m_r = r;
    return *this;
  }
  RGBA &changeG(unsigned char g) {
    m_g = g;
    return *this;
  }
  RGBA &changeB(unsigned char b) {
    m_b = b;
    return *this;
  }
  RGBA &changeA(unsigned char a) {
    m_a = a;
    return *this;
  }

  RGBA &fade_to(const RGBA &color, float p) {
    m_a =
        m_a + static_cast<unsigned char>((color.m_a - m_a) * ((p + 1.0f) / 2));
    m_b =
        m_b + static_cast<unsigned char>((color.m_b - m_b) * ((p + 1.0f) / 2));
    m_g =
        m_g + static_cast<unsigned char>((color.m_g - m_g) * ((p + 1.0f) / 2));
    m_r =
        m_r + static_cast<unsigned char>((color.m_r - m_r) * ((p + 1.0f) / 2));
    return *this;
  }

  /// @brief Get as Uint32
  /// @return color
  uint32_t toRGBA() const { return RGBA8(m_r, m_g, m_b, m_a); }

  // Just calculate the "lightness" f.e. to use Text or Text2
  float luminance() const {
    // For Reference https://en.wikipedia.org/wiki/HSL_and_HSV#Lightness
    return (0.3 * (m_r / 255.f) + 0.59 * (m_g / 255.f) + 0.11 * (m_b / 255.f));
  }

  bool is_light() {
    // Gives us the light or dark to not
    // always use the below "if" statement
    return (luminance() >= 0.5);
  }

  uint8_t m_r, m_g, m_b, m_a;
};
/// @brief Convert RGB to Hex
/// @param r
/// @param g
/// @param b
/// @return Hex-String
std::string RGB2Hex(int r, int g, int b);
/// @brief Hex to U32
/// @param color
/// @param a
/// @return Color32
uint32_t Hex(const std::string &color, uint8_t a = 255);
}  // namespace Color
}  // namespace RenderD7