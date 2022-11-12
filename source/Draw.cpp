#include <renderd7/Draw.hpp>

extern C2D_TextBuf TextBuf;
extern C2D_Font Font;
extern bool currentScreen;

bool RenderD7::Draw::Rect(float x, float y, float w, float h, u32 color) {
  return C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}

bool RenderD7::Draw::Px(float x, float y, u32 color) {
  return C2D_DrawRectSolid(x, y, 0.5f, 1, 1, color);
}

void RenderD7::Draw::TextCentered(float x, float y, float size, u32 color,
                                  std::string Text, int maxWidth, int maxHeight,
                                  C2D_Font fnt) {
  float lineHeight, widthScale;

  // Check for the lineHeight.
  if (fnt != nullptr) {
    lineHeight = RenderD7::Draw::GetTextHeight(size, " ", fnt);
  } else {
    lineHeight = RenderD7::Draw::GetTextHeight(size, " ");
  }

  int line = 0;
  while (Text.find('\n') != Text.npos) {
    if (maxWidth == 0) {
      // Do the widthScale.
      if (fnt != nullptr) {
        widthScale = RenderD7::Draw::GetTextWidth(
            size, Text.substr(0, Text.find('\n')), fnt);
      } else {
        widthScale =
            RenderD7::Draw::GetTextWidth(size, Text.substr(0, Text.find('\n')));
      }
    } else {
      // Do the widthScale 2.
      if (fnt != nullptr) {
        widthScale = std::min((float)maxWidth,
                              RenderD7::Draw::GetTextWidth(
                                  size, Text.substr(0, Text.find('\n')), fnt));
      } else {
        widthScale = std::min((float)maxWidth,
                              RenderD7::Draw::GetTextWidth(
                                  size, Text.substr(0, Text.find('\n'))));
      }
    }
    if (fnt != nullptr) {
      RenderD7::Draw::Text((currentScreen ? 200 : 160) + x - (widthScale / 2),
                           y + (lineHeight * line), size, color,
                           Text.substr(0, Text.find('\n')), maxWidth, maxHeight,
                           fnt);
    } else {
      RenderD7::Draw::Text((currentScreen ? 200 : 160) + x - (widthScale / 2),
                           y + (lineHeight * line), size, color,
                           Text.substr(0, Text.find('\n')), maxWidth,
                           maxHeight);
    }

    Text = Text.substr(Text.find('\n') + 1);
    line++;
  }

  if (maxWidth == 0) {
    // Do the next WidthScale.
    if (fnt != nullptr) {
      widthScale = RenderD7::Draw::GetTextWidth(
          size, Text.substr(0, Text.find('\n')), fnt);
    } else {
      widthScale =
          RenderD7::Draw::GetTextWidth(size, Text.substr(0, Text.find('\n')));
    }
  } else {
    // And again.
    if (fnt != nullptr) {
      widthScale = std::min((float)maxWidth,
                            RenderD7::Draw::GetTextWidth(
                                size, Text.substr(0, Text.find('\n')), fnt));
    } else {
      widthScale = std::min(
          (float)maxWidth,
          RenderD7::Draw::GetTextWidth(size, Text.substr(0, Text.find('\n'))));
    }
  }
  if (fnt != nullptr) {
    RenderD7::Draw::Text((currentScreen ? 200 : 160) + x - (widthScale / 2),
                         y + (lineHeight * line), size, color,
                         Text.substr(0, Text.find('\n')), maxWidth, maxHeight,
                         fnt);
  } else {
    RenderD7::Draw::Text((currentScreen ? 200 : 160) + x - (widthScale / 2),
                         y + (lineHeight * line), size, color,
                         Text.substr(0, Text.find('\n')), maxWidth, maxHeight);
  }
}

// Draw String or Text.
void RenderD7::Draw::Text(float x, float y, float size, u32 color,
                          std::string Text, int maxWidth, int maxHeight,
                          C2D_Font fnt) {
  C2D_Text c2d_text;

  if (fnt != nullptr) {
    C2D_TextFontParse(&c2d_text, fnt, TextBuf, Text.c_str());
  } else {
    C2D_TextFontParse(&c2d_text, Font, TextBuf, Text.c_str());
  }

  C2D_TextOptimize(&c2d_text);

  float heightScale;
  if (maxHeight == 0) {
    heightScale = size;
  } else {
    if (fnt != nullptr) {
      heightScale = std::min(
          size,
          size * (maxHeight / RenderD7::Draw::GetTextHeight(size, Text, fnt)));
    } else {
      heightScale = std::min(
          size, size * (maxHeight / RenderD7::Draw::GetTextHeight(size, Text)));
    }
  }

  if (maxWidth == 0) {
    C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, heightScale,
                 color);
  } else {
    if (fnt != nullptr) {
      C2D_DrawText(
          &c2d_text, C2D_WithColor, x, y, 0.5f,
          std::min(size, size * (maxWidth / RenderD7::Draw::GetTextWidth(
                                                size, Text, fnt))),
          heightScale, color);
    } else {
      C2D_DrawText(
          &c2d_text, C2D_WithColor, x, y, 0.5f,
          std::min(size, size * (maxWidth /
                                 RenderD7::Draw::GetTextWidth(size, Text))),
          heightScale, color);
    }
  }
}
void RenderD7::Draw::TextRight(float x, float y, float size, u32 color,
                               std::string Text, int maxWidth, int maxHeight,
                               C2D_Font fnt) {
  RenderD7::Draw::Text(x - RenderD7::Draw::GetTextWidth(size, Text, fnt), y,
                       size, color, Text, maxWidth, maxHeight, fnt);
}
// Get String or Text Width.
float RenderD7::Draw::GetTextWidth(float size, std::string Text, C2D_Font fnt) {
  float width = 0;
  if (fnt != nullptr) {
    GetTextSize(size, &width, NULL, Text, fnt);
  } else {
    GetTextSize(size, &width, NULL, Text);
  }
  return width;
}

// Get String or Text Size.
void RenderD7::Draw::GetTextSize(float size, float *width, float *height,
                                 std::string Text, C2D_Font fnt) {
  C2D_Text c2d_text;
  if (fnt != nullptr) {
    C2D_TextFontParse(&c2d_text, fnt, TextBuf, Text.c_str());
  } else {
    C2D_TextFontParse(&c2d_text, Font, TextBuf, Text.c_str());
  }
  C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}

// Get String or Text Height.
float RenderD7::Draw::GetTextHeight(float size, std::string Text,
                                    C2D_Font fnt) {
  float height = 0;
  if (fnt != nullptr) {
    GetTextSize(size, NULL, &height, Text.c_str(), fnt);
  } else {
    GetTextSize(size, NULL, &height, Text.c_str());
  }
  return height;
}

Result RenderD7::Draw::LoadFont(C2D_Font &fnt, const char *Path) {
  fnt = C2D_FontLoad(Path); // Only load if found.
  return 0;
}

// Unload a Font.
Result RenderD7::Draw::UnloadFont(C2D_Font &fnt) {
  if (fnt != nullptr) {
    C2D_FontFree(fnt); // Make sure to only unload if not nullptr.
  }
  return 0;
}

bool RenderD7::Draw::Circle(float x, float y, float radius, u32 color) {
  return C2D_DrawCircleSolid(x, y, 0.5f, radius, color);
}

bool RenderD7::Draw::Image(C2D_Image img, float x, float y, float scaleX,
                           float scaleY) {
  return C2D_DrawImageAt(img, x, y, 0.5f, nullptr, scaleX, scaleY);
}

bool RenderD7::Draw::NFRect(float p1x, float p1y, float w, float h, u32 color,
                            float scale) {
  C2D_DrawLine(p1x, p1y, color, w, p1y, color, scale, 1);
  C2D_DrawLine(w, p1y, color, w, h, color, scale, 1);
  C2D_DrawLine(w, h, color, p1x, h, color, scale, 1);
  C2D_DrawLine(p1x, h, color, p1x, p1y, color, scale, 1);
  return true;
}