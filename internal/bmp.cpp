#include "bmp.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <3ds.h>
extern "C"
{
	#include "fs.h"
}

void BMP::PutPixel565(u8* dst, u8 x, u8 y, u16 v){
	dst[(x+(47-y)*48)*3+0]=(v&0x1F)<<3;
	dst[(x+(47-y)*48)*3+1]=((v>>5)&0x3F)<<2;
	dst[(x+(47-y)*48)*3+2]=((v>>11)&0x1F)<<3;
}

void BMP::Save(std::string path, BMP::Bitmap *bitmap)
{
	//Handle fileHandle;
	FS_OpenArchive(&sdmc_archive, ARCHIVE_SDMC);

	
	u32 bytesWritten;
	u8 moltiplier = bitmap->bitperpixel >> 3;
	u8* tempbuf = (u8*)malloc(0x36+(bitmap->width)*(bitmap->height)*moltiplier);
	memset(tempbuf, 0, 0x36+(bitmap->width)*(bitmap->height)*moltiplier);
	tempbuf[0x36+(bitmap->width)*(bitmap->height)*moltiplier]=0;
	//FS_CreateFile(sdmc_archive, path.c_str(), (u16)(0x36+(bitmap->width)*(bitmap->height)*moltiplier))
	//Result ret = FS_OpenFile(fileHandle, sdmc_archive, path.c_str(), (FS_OPEN_READ | FS_OPEN_WRITE), 0);
	if(ret) printf("error");
	*(u16*)&tempbuf[0x0] = 0x4D42;
	*(u32*)&tempbuf[0x2] = 0x36 + (bitmap->width)*(bitmap->height)*moltiplier;
	*(u32*)&tempbuf[0xA] = 0x36;
	*(u32*)&tempbuf[0xE] = 0x28;
	*(u32*)&tempbuf[0x12] = bitmap->width;
	*(u32*)&tempbuf[0x16] = bitmap->height;
	if (moltiplier == 3) *(u32*)&tempbuf[0x1A] = 0x00180001;
	else *(u32*)&tempbuf[0x1A] = 0x00200001;
	*(u32*)&tempbuf[0x22] = (bitmap->width)*(bitmap->height)*moltiplier;
	int i=0;
	while (i<((bitmap->width)*(bitmap->height)*moltiplier)){
		tempbuf[0x36+i] = bitmap->pixels[i];
		i++;
	}
	//FSFILE_Write(fileHandle, &bytesWritten, 0, (u32*)tempbuf, 0x36 + (src->width)*(src->height)*moltiplier, 0x10001);
	FS_Write(sdmc_archive, path.c_str(), (u32*)tempbuf, 0x36 + (src->width)*(src->height)*moltiplier, 0x10001);
	
	free(tempbuf);
	FS_CloseArchive(sdmc_archive);
}