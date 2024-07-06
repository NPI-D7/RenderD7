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
class R2 {
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
  R2() = default;
  ~R2() = default;

  static void Init();

  // Settings
  static void SetFont(Font::Ref fnt);
  static Font::Ref GetFont();
  static void DefaultFont();
  static void DrawNextLined();
  static void OnScreen(R2Screen screen);
  static R2Screen GetCurrentScreen();
  static void SetTextSize(float szs);
  static void DefaultTextSize();
  static float GetTextSize();
  static R7Vec2 GetCurrentScreenSize();
  // Processing
  static void Process();
  static R7Vec2 GetTextDimensions(const std::string& text);
  static std::string WrapText(const std ::string& in, int maxlen);
  static std::string ShortText(const std::string& in, int maxlen);
  // Draw Functions
  static void AddRect(R7Vec2 pos, R7Vec2 size, RD7Color clr);
  static void AddRect(R7Vec2 pos, R7Vec2 size, unsigned int clr);
  static void AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2, RD7Color clr);
  static void AddTriangle(R7Vec2 pos0, R7Vec2 pos1, R7Vec2 pos2,
                          unsigned int clr);
  static void AddText(R7Vec2 pos, const std::string& text, RD7Color clr,
                      RD7TextFlags flags = 0, R7Vec2 tmb = R7Vec2());
  static void AddText(R7Vec2 pos, const std::string& text, unsigned int clr,
                      RD7TextFlags flags = 0, R7Vec2 tmb = R7Vec2());
  static void AddImage(R7Vec2 pos, Image::Ref img);
  static void AddSprite(Sprite::Ref spr);
  static void AddLine(R7Vec2 pos_a, R7Vec2 pos_b, RD7Color clr, int t = 1);
  static void AddLine(R7Vec2 pos_a, R7Vec2 pos_b, unsigned int clr, int t = 1);

 private:
  static const float default_text_size;
  static float text_size;
  static Font::Ref font;
  static std::map<std::string, float> ts;
  static std::map<std::string, int> mln;
  static bool next_lined;
  static std::vector<R2Cmd::Ref> commands;
  static R2Screen current_screen;
};
}  // namespace RenderD7