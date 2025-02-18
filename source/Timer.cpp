#include <renderd7/Timer.hpp>

// Ticks per MSEC
#define TPMS 268111.856

namespace RenderD7 {
    Timer::Timer(bool autostart) {
        if(autostart)
            is_running = true;
        last = svcGetSystemTick();
        current = last;
    }

    void Timer::reset() {
        last = svcGetSystemTick();
        current = last;
    }

    void Timer::tick() {
        if(is_running)
            current = svcGetSystemTick();
    }

    void Timer::pause() {
        is_running = false;
    }

    void Timer::resume() {
        is_running = true;
    }

    bool Timer::running() {
        return is_running;
    }
    
    float Timer::get() {
        return (float)((current-last)/TPMS);
    }

    float Timer::get_live() {
        return (float)((svcGetSystemTick()-last)/TPMS);
    }
}