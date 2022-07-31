#include <renderd7/Fonts/NFontApi.hpp>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include <renderd7/external/stb_truetype.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <renderd7/external/stb_image_write.h>

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
    b_h = 128;
    b_w = 512;
    l_h = 24; /* line height */
    scale = stbtt_ScaleForPixelHeight(&font, l_h);

    stbtt_GetFontVMetrics(&font, &ascent,&decent,&linegap);
    linespace = scale * (ascent - decent + linegap);

	baseline = (int) (ascent*scale);
	height = (int) ((ascent - decent)*scale);
}

unsigned char* RenderD7::NFontApi::GetGlyphBitmap(char glyph)
{
    //stbtt_GetGlyphBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);
    stbtt_GetCodepointBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);
    w = x1-x0;
	h = y1-y0;
    
	unsigned char* bitmap;
    bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, glyph, &w, &h, 0, 0);
    return bitmap;
}

int RenderD7::NFontApi::GetGlyphHeight(char glyph)
{
    stbtt_GetCodepointBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);
    w = x1-x0;
	h = y1-y0;
    return h;
}

int RenderD7::NFontApi::GetGlyphWidth(char glyph)
{
    stbtt_GetCodepointBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);
    w = x1-x0;
	h = y1-y0;
    return w;
}
