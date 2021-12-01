#include "Click.hpp"

namespace rnd7{
    enum class TweenType : int {Position = 1, Color, Alpha};

    enum class TweenLoop : int {None = 1, Loop = 2,};

    enum class TweenDirection : int {Current, Forward, Backward};

    enum class TweenState : int {Playing = 1, Stopped};
    
}
