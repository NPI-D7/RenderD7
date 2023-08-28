#include <renderd7/sound.hpp>

#include <fstream>
#include <string>
#include <cstring>

extern bool isndspinit;
using std::string;

// Reference: http://yannesposito.com/Scratch/en/blog/2010-10-14-Fun-with-wav/
typedef struct _WavHeader {
  char magic[4];   // "RIFF"
  u32 totallength; // Total file length, minus 8.
  char wavefmt[8]; // Should be "WAVEfmt "
  u32 format;      // 16 for PCM format
  u16 pcm;         // 1 for PCM format
  u16 channels;    // Channels
  u32 frequency;   // Sampling frequency
  u32 bytes_per_second;
  u16 bytes_by_capture;
  u16 bits_per_sample;
  char data[4]; // "data"
  u32 bytes_in_data;
} WavHeader;
static_assert(sizeof(WavHeader) == 44, "WavHeader size is not 44 bytes.");

sound::sound(const string &path, int channel, bool toloop) {
  if (isndspinit) {
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspSetOutputCount(2); // Num of buffers

    // Reading wav file
    std::fstream fp(path, std::ios::in | std::ios::binary);

    if (!fp.is_open()) {
      printf("Could not open the WAV file: %s\n", path.c_str());
      return;
    }

    WavHeader wavHeader;
    fp.read(reinterpret_cast<char*>(&wavHeader), sizeof(WavHeader));
    size_t read = fp.tellg();
    if (read != sizeof(wavHeader)) {
      // Short read.
      printf("WAV file header is too short: %s\n", path.c_str());
      fp.close();
      return;
    }

    // Verify the header.
    static const char RIFF_magic[4] = {'R', 'I', 'F', 'F'};
    if (memcmp(wavHeader.magic, RIFF_magic, sizeof(wavHeader.magic)) != 0) {
      // Incorrect magic number.
      printf("Wrong file format.\n");
      fp.close();
      return;
    }

    if (wavHeader.totallength == 0 ||
        (wavHeader.channels != 1 && wavHeader.channels != 2) ||
        (wavHeader.bits_per_sample != 8 && wavHeader.bits_per_sample != 16)) {
      // Unsupported WAV file.
      printf("Corrupted wav file.\n");
      fp.close();
      return;
    }

    // Get the file size.
    fp.seekg(0, std::ios::end);
    dataSize = fp.tellg();
    dataSize -= sizeof(WavHeader);

    // Allocating and reading samples
    data = static_cast<u8 *>(linearAlloc(dataSize));
    fp.seekg(44, std::ios::beg);
    fp.read(reinterpret_cast<char*>(data), dataSize);
    fp.close();
    dataSize /= 2; // FIXME: 16-bit or stereo?

    // Find the right format
    u16 ndspFormat;
    if (wavHeader.bits_per_sample == 8) {
      ndspFormat = (wavHeader.channels == 1) ? NDSP_FORMAT_MONO_PCM8
                                             : NDSP_FORMAT_STEREO_PCM8;
    } else {
      ndspFormat = (wavHeader.channels == 1) ? NDSP_FORMAT_MONO_PCM16
                                             : NDSP_FORMAT_STEREO_PCM16;
    }

    ndspChnReset(channel);
    ndspChnSetInterp(channel, NDSP_INTERP_NONE);
    ndspChnSetRate(channel, float(wavHeader.frequency));
    ndspChnSetFormat(channel, ndspFormat);

    // Create and play a wav buffer
    memset(&waveBuf, 0, sizeof(waveBuf));

    waveBuf.data_vaddr = reinterpret_cast<u32 *>(data);
    waveBuf.nsamples = dataSize / (wavHeader.bits_per_sample >> 3);
    waveBuf.looping = toloop;
    waveBuf.status = NDSP_WBUF_FREE;
    chnl = channel;
  }
}

sound::~sound() {
  if (isndspinit) {
    waveBuf.data_vaddr = 0;
    waveBuf.nsamples = 0;
    waveBuf.looping = false;
    waveBuf.status = 0;
    ndspChnWaveBufClear(chnl);

    if (data) {
      linearFree(data);
    }
  }
}

void sound::play() {
  if (isndspinit) {
    if (!data)
      return;
    DSP_FlushDataCache(data, dataSize);
    ndspChnWaveBufAdd(chnl, &waveBuf);
  }
}

void sound::stop() {
  if (isndspinit) {
    if (!data)
      return;
    ndspChnWaveBufClear(chnl);
  }
}
