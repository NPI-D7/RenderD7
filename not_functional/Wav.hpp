#pragma once

#include <renderd7/music/Music.hpp>
#include <fstream>

#include <renderd7/external/dr_wav.h>

namespace RenderD7 {
    class WavDecoder : public MusicDecoder {
        public:
        WavDecoder() {}
        ~WavDecoder() {}

        int _init(const std::string& path, MusicMeta& meta) override;
        unsigned int _getSampleRate() override;
        unsigned char _getChannels() override;
        size_t _getBufSize() override;
        unsigned long long _decode(signed short* buf_addr) override;
        void _deinit() override;
        size_t _getFileSamples() override;

        private:
        drwav wav;
        const size_t buf_size = 16 * 1024;
    };
}