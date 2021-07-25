#pragma once

#include <3ds.h>
#include <string>

/** Sound Class */
class sound {
public:
        /**
        *  Construct new Soundeffect
        *  path: Path to the .wav file
        *  channel: the channel 1-23
        *  toloop: true:loop the sound, false: don't loop
        */
	sound(const std::string& path, int channel = 1, bool toloop = false);
        /** deconstruct the sound */
	~sound();
        /** play the sound */
	void play();
        /** stop the sound */
	void stop();

private:
	u32 dataSize;
	ndspWaveBuf waveBuf;
	u8* data = NULL;
	int chnl;
};

