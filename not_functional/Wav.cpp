#include <renderd7/music/Wav.hpp>

#ifdef RENDERD7_MUSICDEC
#define DR_WAV_IMPLEMENTATION
#include <renderd7/external/dr_wav.h>

namespace RenderD7 {
int WavDecoder::_init(const std::string &path, MusicMeta& meta) {
  meta.path(path);
  if(path.find_last_of('/') != path.npos)
    meta.name(path.substr(path.find_last_of('/')));
  else
    meta.name(path);
  return !drwav_init_file(&wav, path.c_str(), NULL);
}

unsigned int WavDecoder::_getSampleRate() { return wav.sampleRate; }

unsigned char WavDecoder::_getChannels() { return wav.channels; }

size_t WavDecoder::_getBufSize() { return buf_size; }

unsigned long long WavDecoder::_decode(signed short*buf_addr) {
  unsigned long long bsf = (unsigned long long)buf_size/(unsigned long long)wav.channels;
  
  unsigned long long samples_read = drwav_read_pcm_frames(&wav, bsf, buf_addr);
  samples_read *= (unsigned long long)wav.channels;

  return samples_read;
}

void WavDecoder::_deinit() {
  drwav_uninit(&wav);
}

size_t WavDecoder::_getFileSamples() {
  return wav.totalPCMFrameCount * (size_t)wav.channels;
}

} // namespace RenderD7
#endif