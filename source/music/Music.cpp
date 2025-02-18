#include <3ds.h>

#ifdef RENDERD7_MUSICDEC

#include <cstring>
#include <fstream>
#include <renderd7/Message.hpp>
#include <renderd7/music/Mp3.hpp>
#include <renderd7/music/Music.hpp>
#include <renderd7/music/Vorbis.hpp>
#include <renderd7/thread.hpp>

extern bool isndspinit;

struct PlayerData {
  RenderD7::MusicMeta meta;
  size_t samples_total = 0;
  size_t samples_played = 0;
  size_t samples_per_sec = 0;

  // Song Info
  std::string file = "";

  signed short *buf1 = NULL;
  signed short *buf2 = NULL;
  ndspWaveBuf waveBuf[2] = {0};
  bool last_buf = false;
  int ret = -1;
  bool done = false;
  bool playing = false;

  bool stop = false;
};

int rd7i_player_channel = 8;
PlayerData rd7i_mp_internal_data;
RenderD7::Thread rd7i_mph;

void rd7i_player_init() {
  rd7i_mp_internal_data.samples_total = 0;
  rd7i_mp_internal_data.samples_played = 0;
  rd7i_mp_internal_data.samples_per_sec = 0;
  rd7i_mp_internal_data.buf1 = NULL;
  rd7i_mp_internal_data.buf2 = NULL;
  rd7i_mp_internal_data.last_buf = false;
  rd7i_mp_internal_data.ret = -1;
  rd7i_mp_internal_data.stop = false;
  rd7i_mp_internal_data.done = false;
  rd7i_mp_internal_data.playing = true;
  rd7i_mp_internal_data.file = "";
}

// Non Public Check funcs
extern int rd7i_check_vorbis(const std::string &path);
extern int rd7i_check_flac(const std::string &path);

namespace RenderD7 {
MusicDecoder *MusicDecoder::decoder = nullptr;

void MusicDecoder::LoadFile(const std::string &path) {
  rd7i_mp_internal_data.meta = MusicMeta();  // AutoClean
  std::fstream f(path, std::ios::in | std::ios::binary);
  if (!f.is_open()) return;
  if (!f.good()) return;
  // File Signature
  unsigned int sig;
  f.read(reinterpret_cast<char *>(&sig), sizeof(unsigned int));
  if (f.tellg() != 4) {
    f.close();
    return;
  }
  f.close();
  switch (sig) {
    // "OGG Collection"
    case 0x5367674F:
      if (rd7i_check_vorbis(path) == 0) {
        decoder = new VorbisDecoder();
        if (init(path, rd7i_mp_internal_data.meta)) {
          MusicDecoder::CleanUp();
          return;
        }
      }
      else
        return;  // Err
      break;

    default:
      if ((sig << 16) == 0xFBFF0000 || (sig << 16) == 0xFAFF0000 ||
          (sig << 8) == 0x33444900) {
        decoder = new Mp3Decoder();
        if (init(path, rd7i_mp_internal_data.meta)) {
          RenderD7::PushMessage(
              RenderD7::Message("Music Player", "Failed to load MP3"));
          MusicDecoder::CleanUp();
          return;
        }
        break;
      }
      return;
      break;
  }
}

void MusicDecoder::CleanUp() {
  if (decoder != nullptr) delete decoder;
  decoder = nullptr;
}

static void rd7i_player_cleaner() {
  MusicDecoder::deinit();
  MusicDecoder::CleanUp();
  ndspChnWaveBufClear(rd7i_player_channel);
  linearFree(rd7i_mp_internal_data.buf1);
  linearFree(rd7i_mp_internal_data.buf2);
  rd7i_mp_internal_data.ret = -1;
  // Also clean ret data
  rd7i_mp_internal_data.samples_per_sec = 0;
  rd7i_mp_internal_data.samples_total = 0;
  rd7i_mp_internal_data.samples_played = 0;
  rd7i_mp_internal_data.file = "";
}

static void rd7i_mplayer(RenderD7::Parameter param) {
  MusicDecoder::LoadFile(param.get<std::string>());
  if (MusicDecoder::decoder == nullptr) return;

  rd7i_mp_internal_data.samples_total = 0;
  rd7i_mp_internal_data.samples_played = 0;
  rd7i_mp_internal_data.samples_per_sec = 0;
  rd7i_mp_internal_data.buf1 = NULL;
  rd7i_mp_internal_data.buf2 = NULL;
  rd7i_mp_internal_data.last_buf = false;
  rd7i_mp_internal_data.ret = -1;
  rd7i_mp_internal_data.stop = false;
  rd7i_mp_internal_data.done = false;
  rd7i_mp_internal_data.playing = true;
  rd7i_mp_internal_data.file = "";

  if (MusicDecoder::getFileSamples() != (size_t)-1)
    rd7i_mp_internal_data.samples_total = MusicDecoder::getFileSamples();

  rd7i_mp_internal_data.samples_per_sec =
      MusicDecoder::getSampleRate() * MusicDecoder::getChannels();
  rd7i_mp_internal_data.buf1 = (signed short *)linearAlloc(
      MusicDecoder::getBufSize() * sizeof(signed short));
  rd7i_mp_internal_data.buf2 = (signed short *)linearAlloc(
      MusicDecoder::getBufSize() * sizeof(signed short));
  rd7i_mp_internal_data.file = param.get<std::string>();

  ndspChnReset(rd7i_player_channel);
  ndspChnWaveBufClear(rd7i_player_channel);
  ndspSetOutputMode(NDSP_OUTPUT_STEREO);
  ndspChnSetInterp(rd7i_player_channel, NDSP_INTERP_POLYPHASE);
  ndspChnSetRate(rd7i_player_channel, MusicDecoder::getSampleRate());
  ndspChnSetFormat(rd7i_player_channel, MusicDecoder::getChannels() == 2
                                            ? NDSP_FORMAT_STEREO_PCM16
                                            : NDSP_FORMAT_MONO_PCM16);

  memset(rd7i_mp_internal_data.waveBuf, 0,
         sizeof(rd7i_mp_internal_data.waveBuf));
  rd7i_mp_internal_data.waveBuf[0].nsamples =
      MusicDecoder::decode(&rd7i_mp_internal_data.buf1[0]) /
      MusicDecoder::getChannels();
  rd7i_mp_internal_data.waveBuf[0].data_vaddr = &rd7i_mp_internal_data.buf1[0];
  ndspChnWaveBufAdd(rd7i_player_channel, &rd7i_mp_internal_data.waveBuf[0]);

  rd7i_mp_internal_data.waveBuf[1].nsamples =
      MusicDecoder::decode(&rd7i_mp_internal_data.buf2[0]) /
      MusicDecoder::getChannels();
  rd7i_mp_internal_data.waveBuf[1].data_vaddr = &rd7i_mp_internal_data.buf2[0];
  ndspChnWaveBufAdd(rd7i_player_channel, &rd7i_mp_internal_data.waveBuf[1]);

  // Wait for music to start playing
  while (ndspChnIsPlaying(rd7i_player_channel) == false) {
    // Waiting
  }

  while (rd7i_mp_internal_data.stop == false) {
    svcSleepThread(100 * 1000);
    if (rd7i_mp_internal_data.last_buf == true &&
        rd7i_mp_internal_data.waveBuf[0].status == NDSP_WBUF_DONE &&
        rd7i_mp_internal_data.waveBuf[1].status == NDSP_WBUF_DONE)
      break;

    if (ndspChnIsPaused(rd7i_player_channel) == true ||
        rd7i_mp_internal_data.last_buf == true)
      continue;

    if (rd7i_mp_internal_data.waveBuf[0].status == NDSP_WBUF_DONE) {
      size_t read = MusicDecoder::decode(&rd7i_mp_internal_data.buf1[0]);
      /* The previous block of samples have finished playing,
       * so accumulate them here. */
      rd7i_mp_internal_data.samples_played +=
          rd7i_mp_internal_data.waveBuf[0].nsamples *
          MusicDecoder::getChannels();

      if (read <= 0) {
        rd7i_mp_internal_data.last_buf = true;
        continue;
      } else if (read < MusicDecoder::getBufSize())
        rd7i_mp_internal_data.waveBuf[0].nsamples =
            read / MusicDecoder::getChannels();

      ndspChnWaveBufAdd(rd7i_player_channel, &rd7i_mp_internal_data.waveBuf[0]);
    }

    if (rd7i_mp_internal_data.waveBuf[1].status == NDSP_WBUF_DONE) {
      size_t read = MusicDecoder::decode(&rd7i_mp_internal_data.buf2[0]);
      rd7i_mp_internal_data.samples_played +=
          rd7i_mp_internal_data.waveBuf[0].nsamples *
          MusicDecoder::getChannels();

      if (read <= 0) {
        rd7i_mp_internal_data.last_buf = true;
        continue;
      } else if (read < MusicDecoder::getBufSize())
        rd7i_mp_internal_data.waveBuf[1].nsamples =
            read / MusicDecoder::getChannels();

      ndspChnWaveBufAdd(rd7i_player_channel, &rd7i_mp_internal_data.waveBuf[1]);
    }

    DSP_FlushDataCache(rd7i_mp_internal_data.buf1,
                       MusicDecoder::getBufSize() * sizeof(int16_t));
    DSP_FlushDataCache(rd7i_mp_internal_data.buf2,
                       MusicDecoder::getBufSize() * sizeof(int16_t));
  }
  rd7i_mp_internal_data.samples_played +=
      rd7i_mp_internal_data.waveBuf[0].nsamples * MusicDecoder::getChannels();
  rd7i_mp_internal_data.samples_played +=
      rd7i_mp_internal_data.waveBuf[0].nsamples * MusicDecoder::getChannels();
  rd7i_player_cleaner();
  rd7i_mp_internal_data.playing = false;
  rd7i_mp_internal_data.done = true;
  return;
}

void MusicPlayer::PlayFile(const std::string &path) {
  if (MusicPlayer::IsRunning()) {
    rd7i_mp_internal_data.stop = true;
  }
  while (rd7i_mp_internal_data.playing) {
  }
  RenderD7::Thread::sleep(100);
  rd7i_mph.initialize(rd7i_mplayer, path);
  rd7i_mph.start(true);
}

void MusicPlayer::Play() {
  if (!rd7i_mp_internal_data.playing) {
    ndspChnSetPaused(rd7i_player_channel, false);
    rd7i_mp_internal_data.playing = true;
  }
}

void MusicPlayer::Pause() {
  if (rd7i_mp_internal_data.playing) {
    ndspChnSetPaused(rd7i_player_channel, true);
    rd7i_mp_internal_data.playing = false;
  }
}

void MusicPlayer::Stop() { rd7i_mp_internal_data.stop = true; }

bool MusicPlayer::IsPlaying() { return rd7i_mp_internal_data.playing; }

bool MusicPlayer::IsRunning() { return rd7i_mph.isRunning(); }

int MusicPlayer::PosCurrent() { return rd7i_mp_internal_data.samples_played; }

int MusicPlayer::Total() { return rd7i_mp_internal_data.samples_total; }

int MusicPlayer::SampleRate() { return rd7i_mp_internal_data.samples_per_sec; }

MusicMeta MusicPlayer::GetMeta() { return rd7i_mp_internal_data.meta; }

}  // namespace RenderD7

#endif