/*
	ofxQNX - BlackBerry PlayBook and BlackBerry 10 add-on for openFrameworks

	Copyright (c) 2012, Laurence Muller (www.multigesture.net)
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above copyright
		  notice, this list of conditions and the following disclaimer in the
		  documentation and/or other materials provided with the distribution.
		* Neither the name of the author nor the
		  names of its contributors may be used to endorse or promote products
		  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* 
TODO:
- Implement the SoundPlayer (OpenAL?)
*/

#ifndef OFXQNXSOUNDPLAYER_H_
#define OFXQNXSOUNDPLAYER_H_

#include "ofBaseSoundPlayer.h"

class ofxQNXSoundPlayer : public ofBaseSoundPlayer{
public:
	bool loadSound(string fileName, bool stream = false) { return false; };
	void unloadSound() {};
	void play() {};
	void stop() {};

	void setVolume(float vol) {};
	void setPan(float vol) {};
	void setSpeed(float spd) {};
	void setPaused(bool bP) {};
	void setLoop(bool bLp) {};
	void setMultiPlay(bool bMp) {};
	void setPosition(float pct) {}; // 0 = start, 1 = end;
	void setPositionMS(int ms) {};

	float getPosition() { return 0; };
	int getPositionMS() { return 0; };
	bool getIsPlaying() { return false; };
	float getSpeed() { return 0; };
	float getPan() { return 0; };
	bool getIsPaused() { return false; };
	bool isLoaded() {  return false; };
	float getVolume() {  return 0; };
};

#endif /* OFXQNXSOUNDPLAYER_H_ */
