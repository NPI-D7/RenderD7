#include <renderd7/music/Mp3.hpp>

#ifdef RENDERD7_MUSICDEC
// MP3 ID3V1
std::string rd7i_id3v1_to_string(const std::string& in) {
  std::string ret = "";
  if(in.length() < 31) ret = in;
  return ret;
}

// For MP3 ID3V2 tags
// Split up a number of lines separated by \n, \r, both or just zero byte
// and print out each line with specified prefix.
std::string rd7i_id3v2_to_string(const std::string& prefix, mpg123_string* inlines) {
    size_t i;
    int hadcr = 0, hadlf = 0;
    std::string lines;
    std::string line;
    size_t len = 0;

    if(inlines == nullptr) return "";
    
    if (inlines->fill > 0) {
        lines = inlines->p;
        len = inlines->fill;
    } else {
        return "";
    }

    line = lines;
    for (i = 0; i < len; ++i) {
        if (lines[i] == '\n' || lines[i] == '\r' || lines[i] == 0) {
            char save = lines[i];
            if (save == '\n') {
                ++hadlf;
            }
            if (save == '\r') {
                ++hadcr;
            }
            if ((hadcr || hadlf) && (hadlf % 2 == 0) && (hadcr % 2 == 0)) {
                line = "";
            }

            if (!line.empty()) {
                lines[i] = 0;
                std::string result;
                result = prefix + line;
                line = "";
                lines[i] = save;
                return result;
            }
        } else {
            hadlf = hadcr = 0;
            if (line.empty()) {
                line = lines.substr(i);
            }
        }
    }
    return "";
}

namespace RenderD7 {
int Mp3Decoder::_init(const std::string &path, MusicMeta& meta) {
  int ret = 0;
  int encoding = 0;
  if ((ret = mpg123_init() != MPG123_OK)) return ret;
  if ((hnd = mpg123_new(NULL, &ret)) == NULL) {
    printf("[MPG123]: ERR -> %s\n", mpg123_plain_strerror(ret));
    return ret;
  }
  if (mpg123_open(hnd, path.c_str()) != MPG123_OK ||
      mpg123_getformat(hnd, &rate, &channels, &encoding) != MPG123_OK) {
    printf("[MPG123]: ERR -> %s\n", mpg123_strerror(hnd));
    return -1;
  }

  mpg123_id3v1* v1;
  mpg123_id3v2* v2;
  mpg123_seek(hnd, 0, SEEK_SET);
  if(mpg123_meta_check(hnd) & MPG123_ID3 && mpg123_id3(hnd, &v1, &v2) == MPG123_OK) {
    if(v1 != nullptr) {
      ///// POSSIBLE CRASH /////
      //meta.title(rd7i_id3v1_to_string(v1->title));
      //meta.album(rd7i_id3v1_to_string(v1->album));
      //meta.artist(rd7i_id3v1_to_string(v1->artist));
      //meta.year(rd7i_id3v1_to_string(v1->year));
      meta.mdt("mpg123_id3v1");
    }
    if(v2 != nullptr) {
      meta.title(rd7i_id3v2_to_string("", v2->title));
      meta.album(rd7i_id3v2_to_string("", v2->album));
      meta.artist(rd7i_id3v2_to_string("", v2->artist));
      meta.year(rd7i_id3v2_to_string("", v2->year));
      meta.mdt("mpg123_id3v2");
    }
  }

  mpg123_format_none(hnd);
  mpg123_format(hnd, rate, channels, encoding);
  buf_size = mpg123_outblock(hnd) * 16;

  return ret;
}

unsigned int Mp3Decoder::_getSampleRate() { return rate; }

unsigned char Mp3Decoder::_getChannels(){return channels; }

size_t Mp3Decoder::_getBufSize() {
  return buf_size;
}

unsigned long long Mp3Decoder::_decode(signed short*buf_addr) {
  size_t done = 0;
  mpg123_read(hnd, buf_addr, buf_size, &done);
  return done / (sizeof(int16_t));
}

void Mp3Decoder::_deinit() {
  mpg123_close(hnd);
  mpg123_delete(hnd);
  mpg123_exit();
}

size_t Mp3Decoder::_getFileSamples() {
  off_t len = mpg123_length(hnd);
  if (len != MPG123_ERR) return len * (size_t)channels;
  return -1;  // Not Exist
}

}  // namespace RenderD7

#endif