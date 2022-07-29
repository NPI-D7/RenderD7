#include <renderd7/Sheet.hpp>

RenderD7::Sheet::Sheet()
{
    //
}
RenderD7::Sheet::~Sheet()
{
    //
}

Result RenderD7::Sheet::Load(const char *path)
{
	this->spritesheet = C2D_SpriteSheetLoad(path);
	return 0;
}

void RenderD7::Sheet::Free()
{
	C2D_SpriteSheetFree(this->spritesheet);
}
