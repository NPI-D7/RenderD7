#pragma once

#include <3ds.h>

#include <renderd7/smart_ctor.hpp>
#include <string>

namespace RenderD7 {
/** Sound Class */
class Sound {
 public:
  /// \brief Construct new Soundeffect
  /// \param path Path to the .wav file
  /// \param channel the channel 1-23
  /// \param toloop true:loop the sound, false: don't loop
  Sound(const std::string &path, int channel = 1, bool toloop = false);
  /// @brief Deconstructor
  ~Sound();
  RD7_SMART_CTOR(Sound)
  /// @brief Play the sound
  void Play();
  /// @brief Stop the sound
  void Stop();

 private:
  /// \param dataSize Size of the filedata
  u32 dataSize;
  /// \param waveBuf For ndsp
  ndspWaveBuf waveBuf;
  /// \param data Memmory data of the sound
  uint8_t *data = NULL;
  /// \param chnl Channel of the sound
  int chnl;
};
}  // namespace RenderD7