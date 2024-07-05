#pragma once

#include <map>
#include <renderd7/Color.hpp>
#include <renderd7/Font.hpp>
#include <renderd7/Image.hpp>
#include <renderd7/R7Vec.hpp>
#include <renderd7/Sprite.hpp>
#include <renderd7/smart_ctor.hpp>

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

enum R2Screen {
  R2Screen_Bottom,
  R2Screen_Top,
  // TopRight,
};

namespace RenderD7 {
class R2Base {
 public:
  struct R2Cmd {
    R7Vec2 pos;        //< Position
    R7Vec2 pszs;       //< Position or (TextBox) Size
    R7Vec2 ap;         //< Additional Pos
    unsigned int clr;  //< Color
    bool Screen;       //< TopScreen
    Image::Ref img;    //< Image Reference
    Sprite::Ref spr;   //< Sprite Reference
    // 0 = skip, 1 = rect, 2 = tri, 3 = text,
    // 4 = image, 5 = sprite, 6 = Line
    int type;            //< Command Type
    bool lined = false;  //< Draw Lined Rect/Tri
    // Text Specific
    RD7TextFlags flags;  // Text Flags
    std::string text;    // Text
    RD7_SMART_CTOR(R2Cmd)
  };
  R2Base();
  ~R2Base() = default;
  RD7_SMART_CTOR(R2Base)

  // Settings
  void SetFont(Font::Ref fnt);
  Font::Ref GetFont();
  void DefaultFont();
  void DrawNextLined();
  void OnScreen(R2Screen screen);
  R2Screen GetCurrentScreen();
  void SetTextSize(float szs);
  void DefaultTextSize();
  float GetTextSize();
  R7Vec2 GetCurrentScreenSize();
  // Processing
  void Process();
  R7Vec2 GetTextDimensions(const std::string& text);
  std::string WrapText(const std ::string& in, int maxlen);
  std::string ShortText(const std::string& in, int maxlen);
  // Draw Functions
  void AddRect(R7Vec2 pos, R7Vec2 size, RD7Color clr);
  void AddRect(R7Vec2 pos, R7Vec2 size, unsigned int clr);
  void AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, RD7Color clr);
  void AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, unsigned int clr);
  void AddText(R7Vec2 pos, const std::string& text, RD7Color clr,
               RD7TextFlags flags = 0, R7Vec2 tmb = R7Vec2());
  void AddText(R7Vec2 pos, const std::string& text, unsigned int clr,
               RD7TextFlags flags = 0, R7Vec2 tmb = R7Vec2());
  void AddImage(R7Vec2 pos, Image::Ref img);
  void AddSprite(Sprite::Ref spr);
  void AddLine(R7Vec2 pos_a, R7Vec2 pos_b, RD7Color clr, int t = 1);
  void AddLine(R7Vec2 pos_a, R7Vec2 pos_b, unsigned int clr, int t = 1);

 private:
  const float default_text_size = 0.5f;
  float text_size = 0.5;
  Font::Ref font;
  std::map<std::string, float> ts;
  std::map<std::string, int> mln;
  bool next_lined = false;
  std::vector<R2Cmd::Ref> commands;
  R2Screen current_screen = R2Screen_Bottom;
};
}  // namespace RenderD7