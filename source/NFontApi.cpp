#include <renderd7/Fonts/NFontApi.hpp>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include <renderd7/external/stb_truetype.h>

RenderD7::NFontApi::NFontApi()
{
    
}

RenderD7::NFontApi::~NFontApi()
{
    
}

void RenderD7::NFontApi::LoadTTF(std::string path)
{
    /////READ FILE
    unsigned char* buffer;
    long size = 0;
    FILE *ttf__ = fopen(path.c_str(), "rb");
    fseek(ttf__, 0, SEEK_END);
    size = ftell(ttf__);
    fseek(ttf__, 0, SEEK_SET);
    buffer = (unsigned char*)malloc(size);
    fread(buffer, size, 1, ttf__);
    fclose(ttf__);
    /////Setup Font
    if (!stbtt_InitFont(&font, buffer, 0))
    {
        printf("failed\n");
        status+="failed\n";
        return;
    }
    status+="success!\n";
    l_h = 24; /* line height */
    scale = stbtt_ScaleForPixelHeight(&font, l_h);

    stbtt_GetFontVMetrics(&font, &ascent,&decent,0);
	baseline = (int) (ascent*scale);
	height = (int) ((ascent - decent)*scale);
    
}

std::vector<unsigned char> RenderD7::NFontApi::GetGlyphBitmap(int glyph)
{
    stbtt_GetGlyphBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);
    w = x1-x0;
	h = y1-y0;
    
	std::vector<unsigned char> bitmap;
    bitmap.resize(h*w);
    stbtt_MakeGlyphBitmap(&font, bitmap.data(), w, h, w, scale, scale, glyph);
    return bitmap;
}

int RenderD7::NFontApi::GetGlyphHeight(int glyph)
{
    stbtt_GetGlyphBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);
    w = x1-x0;
	h = y1-y0;
    return h;
}

int RenderD7::NFontApi::GetGlyphWidth(int glyph)
{
    stbtt_GetGlyphBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);
    w = x1-x0;
	h = y1-y0;
    return w;
}