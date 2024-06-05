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
#include <unistd.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#define UNPACK_RGBA(col) (uint8_t)(col >> 24), (col >> 16), (col >> 8), (col)
#define UNPACK_BGRA(col) (uint8_t)(col >> 8), (col >> 16), (col >> 24), (col)

inline uint32_t RGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
#define ISIMPLEPAK(x, y) (((x) & 0xff) << y)
  return (ISIMPLEPAK(r, 0) | ISIMPLEPAK(g, 8) | ISIMPLEPAK(b, 16) |
          ISIMPLEPAK(a, 24));
}

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
  RD7Color_Header,    ///< Header Color (if the header is dark text2 is used)
  RD7Color_Selector,  ///< Selector Color
  RD7Color_SelectorFade,       ///< Selector FadingTo Color
  RD7Color_List0,              ///< List Color1
  RD7Color_List1,              ///< List Color2
  RD7Color_MessageBackground,  ///< Message Background
  RD7Color_Button,             ///< Button Color
  RD7Color_ButtonHovered,      ///< Button Color if Hovered
  RD7Color_ButtonDisabled,     ///< Button Color if disabled
  RD7Color_ButtonActive,       ///< Button Colkor if Clicked
  RD7Color_Checkmark,          ///< Checkbox Checkmark Color
  RD7Color_FrameBg,            ///< Frame Background Color
  RD7Color_FrameBgHovered,     ///< Frame Background Color if hovered
  RD7Color_Progressbar,        ///< Progressbar Color
  /// NON COLOR ///
  RD7Color_Len,  ///< Used to define the lengh of this list
};

namespace RenderD7 {
class Theme {
 public:
  Theme() = default;
  ~Theme() = default;

  void Load(const std::string &path);
  void Default();
  void Save(const std::string &path);

  unsigned int Get(RD7Color clr);
  void Set(RD7Color clr, unsigned int v);
  void Swap(RD7Color a, RD7Color b);
  bool Undo();
  void UndoAll();
  void TextBy(RD7Color bg);
  RD7Color AutoText(RD7Color bg);

  std::vector<unsigned int> &GetTableRef() { return clr_tab; }
  // For Smart Pointer
  using Ref = std::shared_ptr<Theme>;
  static Ref New() { return std::make_shared<Theme>(); }

 private:
  struct change {
    change(RD7Color a, unsigned int f, unsigned int t)
        : clr(a), from(f), to(t) {}
    change(RD7Color a, RD7Color b, unsigned int f, unsigned int t)
        : clr(a), clr2(b), from(f), to(t) {}
    RD7Color clr;
    RD7Color clr2 = 0;  // Used if Swap
    unsigned int from;
    unsigned int to;
  };
  // Use a vector for faster access
  std::vector<unsigned int> clr_tab;
  std::vector<change> changes;
};

Theme::Ref ThemeActive();
/// @brief Change Theme Adress
/// @param theme your adress
void ThemeSet(Theme::Ref theme);
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
    if (!RenderD7::ThemeActive()) return;
    unsigned int col = RenderD7::ThemeActive()->Get(in);
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
std::string RGBA2Hex(unsigned int c32);
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
