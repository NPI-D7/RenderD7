#pragma once

#include <renderd7/music/Music.hpp>
#include <fstream>

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

namespace RenderD7 {
    class VorbisDecoder : public MusicDecoder {
        public:
        VorbisDecoder() {}
        ~VorbisDecoder() {}

        int _init(const std::string& path, MusicMeta& meta) override;
        unsigned int _getSampleRate() override;
        unsigned char _getChannels() override;
        size_t _getBufSize() override;
        unsigned long long _decode(signed short* buf_addr) override;
        void _deinit() override;
        size_t _getFileSamples() override;

        private:
        OggVorbis_File vorbis_file;
        vorbis_info *vi;
        FILE* f; // Only used cause required by lib
        const size_t buf_size = 8 * 4096;
    };
}