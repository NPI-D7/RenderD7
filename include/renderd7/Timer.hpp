#pragma once

#include <3ds.h>

namespace RenderD7 {
    class Timer {
        public:
        Timer(bool autostart = true);
        ~Timer() {}
        void reset();
        void tick();
        void pause();
        void resume();
        float get();
        float get_live();
        bool running();
        private:
        uint64_t last = 0;
        uint64_t current = 0;
        bool is_running = false;
    };
}