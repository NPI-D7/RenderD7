#pragma once

#include <renderd7/music/Music.hpp>
#include <fstream>

#include <mpg123.h>

namespace RenderD7 {
    class Mp3Decoder : public MusicDecoder {
        public:
        Mp3Decoder() {}
        ~Mp3Decoder() {}

        int _init(const std::string& path, MusicMeta& meta) override;
        unsigned int _getSampleRate() override;
        unsigned char _getChannels() override;
        size_t _getBufSize() override;
        unsigned long long _decode(signed short* buf_addr) override;
        void _deinit() override;
        size_t _getFileSamples() override;

        private:
        mpg123_handle *hnd = nullptr;
        size_t buf_size = 0;
        long rate = 0;
        int channels = 0;
    };
}