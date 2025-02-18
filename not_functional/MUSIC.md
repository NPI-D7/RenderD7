# Copy of removed Music.cpp file loaders
```
#include <renderd7/music/Flac.hpp>
#include <renderd7/music/Wav.hpp>
 // "RIFF"
/*case 0x46464952:
// "riff"
case 0x66666972:
// "RIFX"
case 0x58464952:
// "RF64"
case 0x34364652:
// "FORM"
case 0x4D524F46:
  decoder = new WavDecoder();
  if (init(path, rd7i_mp_internal_data.meta)) {
    MusicDecoder::CleanUp();
    return;
  }
  break;*/
// Flac
/*case 0x43614c66:
  decoder = new FlacDecoder();
  if (init(path, rd7i_mp_internal_data.meta)) {
    RenderD7::PushMessage(
        RenderD7::Message("Music Player", "Failed to load FLAC"));
    MusicDecoder::CleanUp();
    return;
  }
  break;*/

// The other one
/*else if (rd7i_check_flac(path) == 0) {
        decoder = new FlacDecoder();
        if (init(path, rd7i_mp_internal_data.meta)) {
          MusicDecoder::CleanUp();
          return;
        }
      }*/
```