#pragma once

#include <citro2d.h>

#include <renderd7/Color.hpp>
#include <renderd7/Image.hpp>
#include <renderd7/R7Vec.hpp>
#include <renderd7/font.hpp>
#include <string>

#define MAKEFLAG(x) (1 << x)

typedef unsigned int RD7TextFlags;

enum RD7TextFlags_ {
  RD7TextFlags_None = 0,  //< Align is Left and Other things are disabled
  RD7TextFlags_AlignRight = MAKEFLAG(0),
  RD7TextFlags_AlignMid = MAKEFLAG(1),
  RD7TextFlags_Shaddow = MAKEFLAG(2),  // TextBuf Killer lol (doubled Text)
  RD7TextFlags_Wrap = MAKEFLAG(3),
  RD7TextFlags_Short = MAKEFLAG(4),
  RD7TextFlags_Scroll = MAKEFLAG(5),
};

namespace RenderD7 {
R7Vec2 GetTextDimensions(const std::string& text);
void CustomTextSize(float size);
void TextDefaultSize();
std::string TextShort(const std::string& in, int max_len);
// Overrite TextBox Size (by default Screenwidth x Text.h)
void TextMaxBox(R7Vec2 size);
void TextDefaultBox();
void TextFont(Font fnt);
void TextDefaultFont();
namespace Draw2 {
void Rect(R7Vec2 pos, R7Vec2 size, unsigned int color, int t = 1);
void RectFilled(R7Vec2 pos, R7Vec2 size, Color4 colors);
void RectFilledSolid(R7Vec2 pos, R7Vec2 size, unsigned int color);
// Wrapper of RectFilledSolid
inline void RFS(R7Vec2 pos, R7Vec2 size, unsigned int color) {
  RectFilledSolid(pos, size, color);
}
void Line(R7Vec2 pos0, R7Vec2 pos1, unsigned int color, int t = 1);
void Triangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, Color3 colors);
void TriangleSolid(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int color);
// Beta and Very unstable
void TriangleLined(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int color,
                   int t = 1);
void Text(R7Vec2 pos, const std::string& text, RD7TextFlags flags = 0);
void Image(RenderD7::Image* img, const R7Vec2& pos = R7Vec2(0, 0),
           const R7Vec2& scale = R7Vec2(1, 1));
}  // namespace Draw2
}  // namespace RenderD7