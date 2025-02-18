#pragma once

#include <citro2d.h>
#include <3ds.h>

#include <renderd7/R7Vec.hpp>
#include <renderd7/nimg.hpp>

#include <string>

namespace RenderD7 {
    class Image {
        public:
        Image();
        ~Image();
        void load(const std::string& path);
        void from_nimg(const nimg& image);

        C2D_Image get();
        C2D_Image& get_ref();
        void set(const C2D_Image& i);
        R7Vec2 get_size();
        bool loaded();

        private:
        void safe_del();
        bool ld = false;
        C2D_Image img;
    };
}