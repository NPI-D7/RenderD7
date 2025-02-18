#include <renderd7/music/Vorbis.hpp>

#ifdef RENDERD7_MUSICDEC

int rd7i_check_vorbis(const std::string &path) {
  // As always forced by libvorbis to not use fstream :(
  FILE *ft = fopen(path.c_str(), "r");
  OggVorbis_File testvf;
  int err;

  if (ft == NULL) return -1;

  err = ov_test(ft, &testvf, NULL, 0);

  ov_clear(&testvf);
  fclose(ft);
  return err;
}

namespace RenderD7 {
int VorbisDecoder::_init(const std::string &path, MusicMeta& meta) {
  meta.path(path);
  if(path.find_last_of('/') != path.npos)
    meta.name(path.substr(path.find_last_of('/')));
  else
    meta.name(path);
  int ret = -1;
  if ((f = fopen(path.c_str(), "rb")) == NULL) return ret;
  if (ov_open(f, &vorbis_file, NULL, 0) < 0) return ret;
  if ((vi = ov_info(&vorbis_file, -1)) == NULL) return ret;
  ret = 0;
  return ret;
}

unsigned int VorbisDecoder::_getSampleRate() { return vi->rate; }

unsigned char VorbisDecoder::_getChannels() { return vi->channels; }

size_t VorbisDecoder::_getBufSize() { return buf_size; }

unsigned long long VorbisDecoder::_decode(signed short*buf_addr) {
  uint64_t samples_read = 0;
  int samples2read = buf_size;
  char *buf = reinterpret_cast<char *>(buf_addr);

  while (samples2read > 0) {
    int cur_section;
    int samples_just_read =
        ov_read(&vorbis_file, buf, samples2read > 4096 ? 4096 : samples2read,
                &cur_section);
    if (samples_just_read < 0)
      return samples_just_read;
    else if (samples_just_read == 0)
      break;  // End of File
    samples_read += samples_just_read;
    samples2read -= samples_just_read;
    buf += samples_just_read;
  }
  // Aka / sizeof(int16_t)
  return samples_read / sizeof(signed short);
}

void VorbisDecoder::_deinit() {
  ov_clear(&vorbis_file);
  fclose(f);
}

size_t VorbisDecoder::_getFileSamples() {
  return ov_pcm_total(&vorbis_file, -1)*vi->channels; 
}

}  // namespace RenderD7

#endif