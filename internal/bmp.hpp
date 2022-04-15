#pragma once
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <3ds.h>

namespace BMP
{
    struct Bitmap{
	    u32 magic;
	    u8* pixels;
	    int width;
	    int height;
	    u16 bitperpixel;
    };
    void PutPixel565(u8* dst, u8 x, u8 y, u16 v);
	void Save(std::string path, BMP::Bitmap *bitmap);
}