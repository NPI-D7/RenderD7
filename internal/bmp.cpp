#include "bmp.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <3ds.h>
#include <jpeglib.h>

extern "C"
{
	#include "fs.h"
}

void BMP::PutPixel565(u8* dst, u8 x, u8 y, u16 v){
	dst[(x+(47-y)*48)*3+0]=(v&0x1F)<<3;
	dst[(x+(47-y)*48)*3+1]=((v>>5)&0x3F)<<2;
	dst[(x+(47-y)*48)*3+2]=((v>>11)&0x1F)<<3;
}

u8* flipBitmap(u8* flip_bitmap, BMP::Bitmap* result){
	int x, y;
	if (result->bitperpixel == 24){
		for (y = 0; y < result->height; y++){
			for (x = 0; x < result->width; x++){
				int idx = (x+y * result->width)*3;
				*(u32*)(&(flip_bitmap[idx])) = ((*(u32*)&(result->pixels[(x + (result->height - y - 1) * result->width)*3]) & 0x00FFFFFF) | (*(u32*)(&(flip_bitmap[idx])) & 0xFF000000));
			}
		}
	}else if(result->bitperpixel == 32){
		for (y = 0; y < result->height; y++){
			for (x = 0; x < result->width; x++){
				*(u32*)(&(flip_bitmap[(x+y * result->width)<<2])) = *(u32*)&(result->pixels[(x + (result->height - y - 1) * result->width)<<2]);
			}
		}
	}
	return flip_bitmap;
}

void BMP::Save(std::string path, BMP::Bitmap *bitmap)
{
	u8 moltiplier = bitmap->bitperpixel >> 3;
		int size_val = (bitmap->width)*(bitmap->height)*moltiplier;
		u8* flip_pixels = (u8*)malloc(size_val);
		flip_pixels = flipBitmap(flip_pixels, bitmap);
		if (moltiplier == 4){ // 32bpp image - Need to delete alpha channel
			u8* tmp = flip_pixels;
			flip_pixels = (u8*)malloc((bitmap->width)*(bitmap->height)*3);
			u32 i = 0;
			u32 j = 0;
			while ((i+1) < size_val){
				flip_pixels[j++] = tmp[i];
				flip_pixels[j++] = tmp[i+1];
				flip_pixels[j++] = tmp[i+2];
				i = i + 4;
			}
			free(tmp);
		}
		BMP::saveJpg((char*)path.c_str(),(u32*)flip_pixels,bitmap->width,bitmap->height);
		free(flip_pixels);
}

void BMP::saveJpg(char *filename, u32 *pixels, u32 width, u32 height)
{
	FILE *outfile = fopen(filename, "wb");
	struct jpeg_error_mgr jerr;
	struct jpeg_compress_struct cinfo;
	JSAMPROW row_pointer[1];
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_EXT_BGR;
	jpeg_set_defaults(&cinfo);
	cinfo.num_components = 3;
	cinfo.dct_method = JDCT_FLOAT;
	jpeg_set_quality(&cinfo, 100, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	while( cinfo.next_scanline < cinfo.image_height ){
		row_pointer[0] = (unsigned char*)&pixels[ (cinfo.next_scanline * cinfo.image_width * cinfo.input_components) >> 2];
		jpeg_write_scanlines( &cinfo, row_pointer, 1 );
	}
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	fclose(outfile);
}