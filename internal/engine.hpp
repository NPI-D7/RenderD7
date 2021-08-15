#pragma once
#include <3ds.h>
#include <citro2d.h>
#imclude <citro3d.h>

enum EngineType
{
    _2D,
    _3D
};
class Game {
    public:
    Game(EngineType, std::string name);
    ~Game();
};
