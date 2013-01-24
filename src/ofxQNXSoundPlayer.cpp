/*
	ofxQNX - BlackBerry PlayBook and BlackBerry 10 add-on for openFrameworks

	Copyright (c) 2012-2013, Laurence Muller (www.multigesture.net)
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

	note: this class is based on:
	- https://github.com/cocos2d/cocos2d-x/blob/master/CocosDenshion/blackberry/SimpleAudioEngine.cpp

*/

#include "ofxQNXSoundPlayer.h"
#include "ofLog.h"

#include <map>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <sys/stat.h>
#include <vorbis/vorbisfile.h>

struct soundData {
	ALuint buffer;
	ALuint source;
	bool   isLooped;
};

typedef map<string, soundData *> EffectsMap;
EffectsMap s_effects;

//
// OpenAL Error Check
//
static int checkALError(const char *funcName)
{
	int err = alGetError();

	if (err != AL_NO_ERROR)
	{
		switch (err)
		{
			case AL_INVALID_NAME:
				fprintf(stderr, "AL_INVALID_NAME in %s\n", funcName);
				break;

			case AL_INVALID_ENUM:
				fprintf(stderr, "AL_INVALID_ENUM in %s\n", funcName);
				break;

			case AL_INVALID_VALUE:
				fprintf(stderr, "AL_INVALID_VALUE in %s\n", funcName);
				break;

			case AL_INVALID_OPERATION:
				fprintf(stderr, "AL_INVALID_OPERATION in %s\n", funcName);
				break;

			case AL_OUT_OF_MEMORY:
				fprintf(stderr, "AL_OUT_OF_MEMORY in %s\n", funcName);
				break;
		}
	}

	return err;
}

//
// OGG support
//
static bool isOGGFile(const char *pszFilePath)
{
	FILE			*file;
	OggVorbis_File   ogg_file;
	int				 result;

	file = fopen(pszFilePath, "rb");
	result = ov_test(file, &ogg_file, 0, 0);
	ov_clear(&ogg_file);

	return (result == 0);
}

static ALuint createBufferFromOGG(const char *pszFilePath)
{
	ALuint 			buffer;
	OggVorbis_File  ogg_file;
	vorbis_info*    info;
	ALenum 			format;
	int 			result;
	int 			section;
	int				err;
	unsigned int 	size = 0;

	if (ov_fopen(pszFilePath, &ogg_file) < 0)
	{
		ov_clear(&ogg_file);
		fprintf(stderr, "Could not open OGG file %s\n", pszFilePath);
		return -1;
	}

	info = ov_info(&ogg_file, -1);

	if (info->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;

	// size = #samples * #channels * 2 (for 16 bit)
	unsigned int data_size = ov_pcm_total(&ogg_file, -1) * info->channels * 2;
	char* data = new char[data_size];

	while (size < data_size)
	{
		result = ov_read(&ogg_file, data + size, data_size - size, 0, 2, 1, &section);
		if (result > 0)
		{
			size += result;
		}
		else if (result < 0)
		{
			delete [] data;
			fprintf(stderr, "OGG file problem %s\n", pszFilePath);
			return -1;
		}
		else
		{
			break;
		}
	}

	if (size == 0)
	{
		delete [] data;
		fprintf(stderr, "Unable to read OGG data\n");
		return -1;
	}

	// clear al errors
	checkALError("createBufferFromOGG");

	// Load audio data into a buffer.
	alGenBuffers(1, &buffer);

	if (checkALError("createBufferFromOGG") != AL_NO_ERROR)
	{
		fprintf(stderr, "Couldn't generate a buffer for OGG file\n");
		delete [] data;
		return buffer;
	}

	alBufferData(buffer, format, data, data_size, info->rate);
	checkALError("createBufferFromOGG");

	delete [] data;
	ov_clear(&ogg_file);

	return buffer;
}

//--------------------------------------------------------------
ofxQNXSoundPlayer::ofxQNXSoundPlayer() {
	ofLogNotice("ofxQNXSoundPlayer") << "ofxQNXSoundPlayer()";
}

//--------------------------------------------------------------
ofxQNXSoundPlayer::~ofxQNXSoundPlayer() {
	ofLogNotice("ofxQNXSoundPlayer") << "~ofxQNXSoundPlayer()";
}

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::loadSound(string fileName, bool stream) {
	ofLogNotice("ofxQNXSoundPlayer") << "loadSound: " << fileName;

	qnxIsStream = stream;
	qnxFileName = fileName;

	qnxIsLoaded = false;
	qnxLoop = false;
	qnxMultiPlay = false;
	qnxVolume = 1.0f;

	if(qnxIsStream) {
		ofLogWarning("ofxQNXSoundPlayer") << "stream not implemented";
	}
	else {

		EffectsMap::iterator iter = s_effects.find(qnxFileName);

		// check if we have this already
		if (iter == s_effects.end())
		{
			ALuint 		buffer;
			ALuint 		source;
			soundData  *data = new soundData;
			string 	    path = qnxFileName;

			checkALError("preloadEffect");

			if (isOGGFile(path.data()))	{
				buffer = createBufferFromOGG(path.data());
			}
			else {
				buffer = alutCreateBufferFromFile(path.data());
				checkALError("preloadEffect");
			}

			if (buffer == AL_NONE) {
				fprintf(stderr, "Error loading file: '%s'\n", path.data());
				alDeleteBuffers(1, &buffer);
				return qnxIsLoaded;
			}

			alGenSources(1, &source);

			if (checkALError("preloadEffect") != AL_NO_ERROR) {
				alDeleteBuffers(1, &buffer);
				return qnxIsLoaded;
			}

			alSourcei(source, AL_BUFFER, buffer);
			checkALError("preloadEffect");

			data->isLooped = false;
			data->buffer = buffer;
			data->source = source;

			s_effects.insert(EffectsMap::value_type(qnxFileName, data));

			qnxIsLoaded = true;
		}
	}

	return qnxIsLoaded;
};

//--------------------------------------------------------------
void ofxQNXSoundPlayer::unloadSound() {
	ofLogNotice("ofxQNXSoundPlayer") << "unloadSound() " << qnxFileName;

	if(qnxIsStream) {
		ofLogWarning("ofxQNXSoundPlayer") << "stream not implemented";
	}
	else {
		EffectsMap::iterator iter = s_effects.find(qnxFileName);

		if (iter != s_effects.end()) {
			checkALError("unloadEffect");

			alSourceStop(iter->second->source);
			checkALError("unloadEffect");

			alDeleteSources(1, &iter->second->source);
			checkALError("unloadEffect");

			alDeleteBuffers(1, &iter->second->buffer);
			checkALError("unloadEffect");
			delete iter->second;

			s_effects.erase(iter);
		}
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::play() {
	ofLogNotice("ofxQNXSoundPlayer") << "play: " << qnxFileName;


	if(qnxIsStream) {
		ofLogWarning("ofxQNXSoundPlayer") << "stream not implemented";
	}
	else {

		EffectsMap::iterator iter = s_effects.find(qnxFileName);

		if (iter == s_effects.end())
		{
			loadSound(qnxFileName, qnxIsStream);

			// let's try again
			iter = s_effects.find(qnxFileName);
			if (iter == s_effects.end())
			{
				fprintf(stderr, "could not find play sound %s\n", qnxFileName.c_str());
				//return -1;
				return;
			}
		}

		if(!qnxMultiPlay) {
			stop();
		}

		checkALError("playEffect");
		iter->second->isLooped = qnxLoop;
		alSourcei(iter->second->source, AL_LOOPING, iter->second->isLooped ? AL_TRUE : AL_FALSE);
		alSourcePlay(iter->second->source);
		checkALError("playEffect");

		qnxSoundId = iter->second->source;
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::stop() {
	ofLogNotice("ofxQNXSoundPlayer") << "stop: " << qnxFileName;

	if(qnxIsStream) {
		ofLogWarning("ofxQNXSoundPlayer") << "stream not implemented";
	}
	else {
		alSourceStop(qnxSoundId);
		checkALError("stopEffect");
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setVolume(float vol) {
	ofLogNotice("ofxQNXSoundPlayer") << "setVolume: " << vol;
	if(qnxIsStream) {
		ofLogWarning("ofxQNXSoundPlayer") << "stream not implemented";
	}
	else {
		if (vol != qnxVolume) {
			EffectsMap::const_iterator end = s_effects.end();
			for (EffectsMap::const_iterator it = s_effects.begin(); it != end; it++) {
				alSourcef(it->second->source, AL_GAIN, vol);
			}

			qnxVolume = vol;
		}
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPan(float vol) {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: setPan";
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setSpeed(float spd) {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: setSpeed";
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPaused(bool bP) {
	ofLogNotice("ofxQNXSoundPlayer") << "setPaused()";

	if(qnxIsStream) {
		ofLogWarning("ofxQNXSoundPlayer") << "stream not implemented";
	}
	else {

		if(bP) {
			if(getIsPlaying()) {
				alSourcePause(qnxSoundId);
				checkALError("pauseEffect");
			}
		}
		else {
			alSourcePlay(qnxSoundId);
			checkALError("resumeEffect");
		}
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setLoop(bool bLp) {
	qnxLoop = bLp;
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setMultiPlay(bool bMp) {
	qnxMultiPlay = bMp;
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPosition(float pct) {
	// 0 = start, 1 = end;
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: setPosition";
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPositionMS(int ms) {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: setPositionMS";
}

//--------------------------------------------------------------
float ofxQNXSoundPlayer::getPosition() {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: getPosition";
	return 0;
}

//--------------------------------------------------------------
int ofxQNXSoundPlayer::getPositionMS() {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: getPositionMS";
	return 0;
}

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::getIsPlaying() {
	ofLogNotice("ofxQNXSoundPlayer") << "getIsPlaying()";

	if(qnxIsStream) {
		ofLogWarning("ofxQNXSoundPlayer") << "stream not implemented";
		return false;
	}
	else {
		ALint play_status;
		alGetSourcei(qnxSoundId, AL_SOURCE_STATE, &play_status);

		return (play_status == AL_PLAYING);
	}

	return false;
}

//--------------------------------------------------------------
float ofxQNXSoundPlayer::getSpeed() {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: getSpeed";
	return 0;
}

//--------------------------------------------------------------
float ofxQNXSoundPlayer::getPan() {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: getPan";
	return 0;
}

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::getIsPaused() {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: getIsPaused";
	return false;
}

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::isLoaded() {
	return qnxIsLoaded;
}

//--------------------------------------------------------------
float ofxQNXSoundPlayer::getVolume() {
	return qnxVolume;
}
