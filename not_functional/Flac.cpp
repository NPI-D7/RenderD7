#include <renderd7/music/Flac.hpp>
#ifdef RENDERD7_MUSICDEC
#define DR_FLAC_IMPLEMENTATION
#include <renderd7/external/dr_flac.h>

int rd7i_check_flac(const std::string &path) {
  int err = -1;
	drflac* flac = drflac_open_file(path.c_str(), NULL);

	if(flac != NULL)
		err = 0;

	drflac_close(flac);
	return err;
}

namespace RenderD7 {
int FlacDecoder::_init(const std::string &path, MusicMeta& meta) {
  meta.path(path);
  if(path.find_last_of('/') != path.npos)
    meta.name(path.substr(path.find_last_of('/')));
  else
    meta.name(path);
  flac = drflac_open_file(path.c_str(), NULL);
  return flac == NULL ? -1 : 0;
}

unsigned int FlacDecoder::_getSampleRate() { return flac->sampleRate; }

unsigned char FlacDecoder::_getChannels() { return flac->channels; }

size_t FlacDecoder::_getBufSize() { return buf_size; }

unsigned long long FlacDecoder::_decode(signed short*buf_addr) {
  unsigned long long bsf = (unsigned long long)buf_size/(unsigned long long)flac->channels;
  
  unsigned long long samples_read = drflac_read_pcm_frames_s16(flac, bsf, buf_addr);
  samples_read *= (unsigned long long)flac->channels;

  return samples_read;
}

void FlacDecoder::_deinit() {
  drflac_close(flac);
}

size_t FlacDecoder::_getFileSamples() {
  return flac->totalPCMFrameCount * (size_t)flac->channels;
}

} // namespace RenderD7
#endif