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

	notes: this class is based on CocosDenshion:
	- Source https://github.com/cocos2d/cocos2d-x/blob/master/CocosDenshion/blackberry/SimpleAudioEngine.cpp

	Sound FX notes:
	- OpenAL is initiated and stopped in ofAppQNXWindow.cpp
	- Supports Wav and Ogg only

	Stream notes:
	- Implemented using the Multimedia Renderer (Based on PlayAudio example and Cocos2d-x class)
	- The stream implementation is working although only one stream can be active at the time (acts as background music)
	- Supported formats: https://developer.blackberry.com/devzone/develop/supported_media/bb_media_support_at_a_glance.html#kba1328545526665
*/

#include "ofxQNXSoundPlayer.h"
#include "ofLog.h"

#include <map>

#include <AL/alut.h>
#include <vorbis/vorbisfile.h>

#include <sys/neutrino.h>
#include <sys/stat.h>

#include <mm/renderer.h>
#include <mm/renderer/events.h>

//
// Sound FX
//
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
	if (err != AL_NO_ERROR) {
		switch (err) {
			case AL_INVALID_NAME:
				ofLogError("checkALError") << "AL_INVALID_NAME in " << funcName;
				break;

			case AL_INVALID_ENUM:
				ofLogError("checkALError") << "AL_INVALID_ENUM in " << funcName;
				break;

			case AL_INVALID_VALUE:
				ofLogError("checkALError") << "AL_INVALID_VALUE in " << funcName;
				break;

			case AL_INVALID_OPERATION:
				ofLogError("checkALError") << "AL_INVALID_OPERATION in " << funcName;
				break;

			case AL_OUT_OF_MEMORY:
				ofLogError("checkALError") << "AL_OUT_OF_MEMORY in " << funcName;
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
		ofLogError("ofxQNXSoundPlayer") << "Could not open OGG file " << pszFilePath;
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
			ofLogError("ofxQNXSoundPlayer") << "OGG file problem " << pszFilePath;
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
		ofLogError("ofxQNXSoundPlayer") << "Unable to read OGG data";
		return -1;
	}

	// clear al errors
	checkALError("createBufferFromOGG");

	// Load audio data into a buffer.
	alGenBuffers(1, &buffer);

	if (checkALError("createBufferFromOGG") != AL_NO_ERROR)
	{
		ofLogError("ofxQNXSoundPlayer") << "Couldn't generate a buffer for OGG file";
		delete [] data;
		return buffer;
	}

	alBufferData(buffer, format, data, data_size, info->rate);
	checkALError("createBufferFromOGG");

	delete [] data;
	ov_clear(&ogg_file);

	return buffer;
}

//
// Multimedia Renderer (Stream)
//
static void mmrerror( mmr_context_t *ctxt, const char *errmsg ) {
	const mmr_error_info_t *err = mmr_error_info( ctxt );
	unsigned errcode = err->error_code;
	ofLogError("ofxQNXSoundPlayer") << "mmrerror " << errmsg << ": error " << errcode;
}

typedef enum {
	PLAYING,
	STOPPED,
	PAUSED,
} playStatus;

static bool s_isBackgroundInitialized = false;
static std::string s_currentBackgroundStr;
static int s_audioOid;

static bool s_hasMMRError = false;
static playStatus s_playStatus = STOPPED;

static mmr_connection_t   *s_mmrConnection 	  = 0;
static mmr_context_t 	  *s_mmrContext 	  = 0;
static strm_dict_t 		  *s_repeatDictionary = 0;
static strm_dict_t 		  *s_volumeDictionary = 0;

//--------------------------------------------------------------
ofxQNXSoundPlayer::ofxQNXSoundPlayer() {
}

//--------------------------------------------------------------
ofxQNXSoundPlayer::~ofxQNXSoundPlayer() {
}

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::loadSound(string fileName, bool stream) {
	qnxIsStream = stream;
	qnxFileName = fileName;

	qnxIsLoaded = false;
	qnxLoop = false;
	qnxMultiPlay = false;
	qnxVolume = 1.0f;

	if(qnxIsStream) {
		ofLogNotice("ofxQNXSoundPlayer") << "Load stream: " << qnxFileName;

		s_playStatus = STOPPED;

		if (!s_isBackgroundInitialized)	{
			char cwd[FILENAME_MAX];
			memset(cwd, FILENAME_MAX, 0);
			getcwd(cwd, PATH_MAX);

			char path[PATH_MAX];
			snprintf(path, PATH_MAX, "file://%s%s", cwd, qnxFileName.c_str());

			s_mmrConnection = mmr_connect(NULL);
			if (!s_mmrConnection) {
				perror("mmr_connect");
				s_hasMMRError = true;
				return qnxIsLoaded;
			}

			const char *ctxtname = "mmrplayer";
			s_mmrContext = mmr_context_create(s_mmrConnection, ctxtname, 0, S_IRUSR | S_IXUSR);
			if (!s_mmrContext) {
				perror(ctxtname);
				s_hasMMRError = true;
				return qnxIsLoaded;
			}

			if ((s_audioOid = mmr_output_attach(s_mmrContext, "audio:default", "audio")) < 0) {
				mmrerror(s_mmrContext, "audio:default");
				return qnxIsLoaded;
			}

			if (mmr_input_attach(s_mmrContext, path, "autolist") < 0) {
				ofLogError("ofxQNXSoundPlayer") << "unable to load " << path;
				mmrerror(s_mmrContext, path);
				return qnxIsLoaded;
			}
		}

		s_currentBackgroundStr 	  = qnxFileName;
		s_isBackgroundInitialized = true;

		qnxIsLoaded = true;
	}
	else {
		ofLogNotice("ofxQNXSoundPlayer") << "Load effect: " << fileName;

		EffectsMap::iterator iter = s_effects.find(qnxFileName);

		// Check if it is already stored
		if (iter == s_effects.end()) {
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
				ofLogError("ofxQNXSoundPlayer") << "Error loading file: " << path.data();
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
	if(qnxIsStream) {
		qnxStopBackground(true);
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
	if(qnxIsStream) {
		ofLogNotice("ofxQNXSoundPlayer") << "stream play: " << qnxFileName;

		if (0 != strcmp(s_currentBackgroundStr.c_str(), qnxFileName.c_str())) {
			qnxStopStream(true);
		}
		else {
			if (s_playStatus == PAUSED) {
				qnxResumeStream();
			}
			else {
				qnxRewindStream();
			}
		}

		if (!s_isBackgroundInitialized) {
			loadSound(qnxFileName, true);
		}

		if(qnxLoop) {
			// set it up to loop
			strm_dict_t *dictionary = strm_dict_new();
			s_repeatDictionary = strm_dict_set(dictionary, "repeat", "all");

			if (mmr_input_parameters(s_mmrContext, s_repeatDictionary) != 0) {
				mmrerror(s_mmrContext, "input parameters (loop)");
				return;
			}
		}

		if (s_hasMMRError || !s_mmrContext) {
			return;
		}

		if (mmr_play(s_mmrContext) < 0) {
			mmrerror(s_mmrContext, "mmr_play");
			s_hasMMRError = true;
		}

		if (!s_hasMMRError) {
			s_playStatus = PLAYING;
		}
	}
	else {

		EffectsMap::iterator iter = s_effects.find(qnxFileName);
		if (iter == s_effects.end()) {
			loadSound(qnxFileName, qnxIsStream);

			// let's try again
			iter = s_effects.find(qnxFileName);
			if (iter == s_effects.end()) {
				ofLogError("ofxQNXSoundPlayer") << "could not find play sound " << qnxFileName.c_str();
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

		qnxSoundId = iter->second->source;	// Store Id to control this fx
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::stop() {
	if(qnxIsStream) {
		qnxStopStream(true);
	}
	else {
		alSourceStop(qnxSoundId);
		checkALError("stopEffect");
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setVolume(float vol) {
	qnxVolume = vol;

	if(qnxIsStream) {
		qnxSetStreamVolume(qnxVolume);
	}
	else {
		alSourcef(qnxSoundId, AL_GAIN, qnxVolume);
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPan(float vol) {
	ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: setPan";
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setSpeed(float spd) {
	if(qnxIsStream) {
		if(mmr_speed_set(s_mmrContext, (spd * 1000)) < 0) {
			mmrerror(s_mmrContext, "mmr_speed_set");
			s_hasMMRError = true;
		}
	}
	else {
		ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: setSpeed";
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPaused(bool bP) {

	if(qnxIsStream) {
		if(bP) {
			if(getIsPlaying()) {
				qnxPauseStream();
			}
		}
		else {
			qnxResumeStream();
		}
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

	if(qnxIsStream) {
		char position[PATH_MAX];
		snprintf(position, PATH_MAX, "1:%d", ms);
		ofLogWarning("ofxQNXSoundPlayer") << "setPositionMS" << position;

		//if (s_mmrContext && mmr_seek(s_mmrContext, "1:0") < 0)
		if (s_mmrContext && mmr_seek(s_mmrContext, position) < 0) {
			mmrerror(s_mmrContext, "rewind");
		}
	}
	else {
		ofLogWarning("ofxQNXSoundPlayer") << "Method not implemented: setPositionMS";
	}
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
	if(qnxIsStream) {
		// TODO update when finished playing
		return (s_playStatus == PLAYING) && s_isBackgroundInitialized;
	}
	else {
		ALint play_status;
		alGetSourcei(qnxSoundId, AL_SOURCE_STATE, &play_status);

		return (play_status == AL_PLAYING);
	}
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

//--------------------------------------------------------------
void ofxQNXSoundPlayer::qnxStopBackground(bool bReleaseData) {
	s_playStatus = STOPPED;

	if (s_mmrContext) {
		mmr_stop(s_mmrContext);
	}

	if (bReleaseData) {
		if (s_mmrContext) {
			mmr_input_detach(s_mmrContext);
			mmr_context_destroy(s_mmrContext);
		}
		if (s_mmrConnection) {
			mmr_disconnect(s_mmrConnection);
		}
		if (s_repeatDictionary) {
			strm_dict_destroy(s_repeatDictionary);
		}
		if (s_volumeDictionary) {
			strm_dict_destroy(s_volumeDictionary);
		}

		s_mmrContext = 0;
		s_mmrConnection = 0;
		s_repeatDictionary = 0;
		s_volumeDictionary = 0;
		s_hasMMRError = false;
		s_currentBackgroundStr = "";
		s_isBackgroundInitialized = false;
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::qnxResumeStream() {
	if (s_mmrContext && mmr_speed_set(s_mmrContext, 1000) < 0) {
		mmrerror(s_mmrContext, "resume");
	}

	s_playStatus = PLAYING;
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::qnxRewindStream() {
	if (s_mmrContext && mmr_seek(s_mmrContext, "1:0") < 0) {
		mmrerror(s_mmrContext, "rewind");
	}
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::qnxStopStream(bool bReleaseData) {
	if (s_playStatus == PAUSED) {
		qnxResumeStream();
	}
	qnxStopBackground(bReleaseData);
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::qnxPauseStream()
{
	if (s_mmrContext && mmr_speed_set(s_mmrContext, 0) < 0)	{
		mmrerror(s_mmrContext, "pause");
	}

	s_playStatus = PAUSED;
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::qnxSetStreamVolume(float volume)
{
	if (!s_isBackgroundInitialized) {
		return;
	}

	// set it up the background volume
	strm_dict_t *dictionary = strm_dict_new();

	char volume_str[128];
	sprintf(volume_str, "%d", (int)(volume * 100) );
	s_volumeDictionary = strm_dict_set(dictionary, "volume", volume_str);

	if (mmr_output_parameters(s_mmrContext, s_audioOid, s_volumeDictionary) != 0) {
		mmrerror(s_mmrContext, "output parameters");
		return;
	}
}
