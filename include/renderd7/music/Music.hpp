// RenderD7 Music Backend
#pragma once

#include <string>

namespace RenderD7 {
class MusicMeta {
 public:
  MusicMeta() {
    ititle = "Unknown";
    ialbum = "Unknown";
    iyear = "Unknown";
    iname = "Unknown";
    ipath = "Unknown";
    iartist = "Unknown";
    imdt = "Unknown";
  }
  ~MusicMeta() {}
  std::string name() { return iname; }
  std::string album() { return ialbum; }
  std::string year() { return iyear; }
  std::string title() { return ititle; }
  std::string path() { return ipath; }
  std::string artist() { return iartist; }
  std::string mdt() { return imdt; }

  void name(std::string name) { iname = name; }
  void album(std::string album) { ialbum = album; }
  void year(std::string year) { iyear = year; }
  void title(std::string title) { ititle = title; }
  void path(std::string path) { ipath = path; }
  void artist(std::string artist) { iartist = artist; }
  void mdt(std::string mdt) { imdt = mdt; }

 private:
  std::string ititle = "";
  std::string ialbum = "";
  std::string iyear = "";
  std::string iname = "";
  std::string ipath = "";
  std::string iartist = "";
  std::string imdt = "";
};

// Template Class for Decoders
class MusicDecoder {
 public:
  MusicDecoder() = default;
  virtual ~MusicDecoder() {}

  static void LoadFile(const std::string &path);
  static void CleanUp();

  static int init(const std::string &path, MusicMeta &meta) {
    return decoder->_init(path, meta);
  }
  static unsigned int getSampleRate() { return decoder->_getSampleRate(); }
  static unsigned char getChannels() { return decoder->_getChannels(); }
  static size_t getBufSize() { return decoder->_getBufSize(); }
  static unsigned long long decode(signed short *buf_addr) {
    return decoder->_decode(buf_addr);
  }
  static void deinit() { decoder->_deinit(); }
  static size_t getFileSamples() { return decoder->_getFileSamples(); }

  static MusicDecoder *decoder;

 protected:
  virtual int _init(const std::string &path, MusicMeta &meta) = 0;
  virtual unsigned int _getSampleRate() = 0;
  virtual unsigned char _getChannels() = 0;
  virtual size_t _getBufSize() = 0;
  virtual unsigned long long _decode(signed short *buf_addr) = 0;
  virtual void _deinit() = 0;
  virtual size_t _getFileSamples() = 0;
};

namespace MusicPlayer {
void PlayFile(const std::string &path);
void Play();
void Pause();
void Stop();
bool IsPlaying();
bool IsRunning();
int PosCurrent();
int Total();
int SampleRate();
MusicMeta GetMeta();
}  // namespace MusicPlayer
}  // namespace RenderD7