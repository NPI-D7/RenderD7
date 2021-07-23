//
//  TweenPaths.cpp
//
//  This code is derived from Universal Tween Engine
//  Licensed under Apache License 2.0 - http://www.apache.org/licenses/LICENSE-2.0
//

#include <TweenEngine/TweenPaths.h>

#include <TweenEngine/paths/LinearPath.h>
#include <TweenEngine/paths/CatmullRom.h>

namespace TweenEngine
{
    TweenPath &TweenPaths::linear = *(new LinearPath());
    TweenPath &TweenPaths::catmullRom = *(new CatmullRom());
}