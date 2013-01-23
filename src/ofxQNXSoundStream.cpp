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


	possibly useful: http://www.gamedev.net/topic/602042-manipulating-sounds-c/
	TODO: Clean up class and remove SDL dependency
 */

#include "ofSoundStream.h"
#include "ofMath.h"
#include "ofUtils.h"

#include <sys/asoundlib.h>

#include "ofxQNXSoundStream.h"

SDL_AudioSpec * desired;
SDL_AudioSpec * obtained;

static int nInputChannels;
static int nOutputChannels;
static int sampleRate;
static int bufferSize;
static int outputBufferSize;

static float * out_float_buffer = NULL;
static Sint16 * out_buffer = NULL;

static bool isSetup = false;
static bool isRunning = false;
static ofBaseSoundInput * soundInputPtr = NULL;
static ofBaseSoundOutput * soundOutputPtr = NULL;

//--------------------------------------------------------------
int ofxQNXSoundStream::openQNXAudio() {

	ofLogNotice("ofxQNXSoundStream") << "openQNXAudio()";

    if( SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO ) <0 ) {
    	ofLogNotice("ofxQNXSoundStream") << "Unable to init SDL: " << SDL_GetError();
    }

	// Allocate a desired SDL_AudioSpec
	desired = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));

	// Allocate space for the obtained SDL_AudioSpec
	obtained = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));

	// choose a samplerate and audio-format
	desired->freq = sampleRate;
	desired->format = AUDIO_S16LSB;

	desired->samples = bufferSize;

	// Our callback function
	desired->callback = AudioCallback;
	desired->userdata = this;
	desired->channels = nOutputChannels;

	// Open the audio device and start playing sound!
	if ( SDL_OpenAudio(desired, obtained) < 0 ) {
		ofLogNotice("ofxQNXSoundStream") << "AudioMixer, Unable to open audio: " << SDL_GetError();
	}

    nOutputChannels = obtained->channels;
	bufferSize = obtained->samples;
	sampleRate = obtained->freq;

	// if the format is 16 bit, two bytes are written for every sample
	if (obtained->format==AUDIO_U16 || obtained->format==AUDIO_S16) {
		outputBufferSize = 2 * bufferSize;
	}
	else {
		outputBufferSize = bufferSize;
	}

	ofLogNotice("ofxQNXSoundStream") << "bufferSize: " << bufferSize;
	ofLogNotice("ofxQNXSoundStream") << "outputBufferSize: " << outputBufferSize;
	ofLogNotice("ofxQNXSoundStream") << "sampleRate: " << sampleRate;
	ofLogNotice("ofxQNXSoundStream") << "channels: " << nOutputChannels;

    char name[32];
    ofLogNotice("ofxQNXSoundStream") << "Using audio driver: " << SDL_AudioDriverName(name, 32);

    out_float_buffer = new float[outputBufferSize];
	out_buffer = new Sint16[outputBufferSize];

	//4096 - playBook (84 - 97) / iOS (not supported)
	//2048 - playBook (46-48) / iOS (46-48)
	//1024 - playbook (24) / iOS (24)
	//512 - playbook (0 or 24) / iOS (11-12)
	//256 - playbook () / iOS (5-6)
	//128 - playbook () / iOS (doesn't support)

    /* Let the audio run */
	SDL_PauseAudio (0);

	return 1;
}

//--------------------------------------------------------------
void ofxQNXSoundStream::AudioCallback(void *userdata, uint8_t *stream, int len){

	soundOutputPtr->audioOut(out_float_buffer, bufferSize, nOutputChannels);

	Sint16 * out_buffer = (Sint16 *)stream; //this is important - view http://lists.libsdl.org/pipermail/sdl-libsdl.org/2008-October/066797.html
	for(int i=0; i< outputBufferSize; i++) {
		 float tempf = (out_float_buffer[i] * 32767.5f) - 0.5f;
		 out_buffer[i] = tempf;
	}

	memcpy(stream, out_buffer, outputBufferSize);
}

//--------------------------------------------------------------
static void closeQNXAudio() {
	ofLogNotice("ofxQNXSoundStream") << "closeQNXAudio()";

	/*snd_pcm_plugin_flush(pcm_handle, SND_PCM_CHANNEL_PLAYBACK);
	snd_mixer_close(mixer_handle);
	snd_pcm_close(pcm_handle);*/
}

//--------------------------------------------------------------
ofxQNXSoundStream::ofxQNXSoundStream() {
	//ofLogNotice("ofxQNXSoundStream") << "ofxQNXSoundStream()";
}

//--------------------------------------------------------------
ofxQNXSoundStream::~ofxQNXSoundStream() {
	ofLogNotice("ofxQNXSoundStream") << "~ofxQNXSoundStream()";
	//closeQNXAudio(); // causes segfault
}

//--------------------------------------------------------------
void ofxQNXSoundStream::setInput(ofBaseSoundInput * soundInput) {
	ofLogNotice("ofxQNXSoundStream") << "setInput(ofBaseSoundInput)";
	soundInputPtr = soundInput;
}

//--------------------------------------------------------------
void ofxQNXSoundStream::setOutput(ofBaseSoundOutput * soundOutput) {
	ofLogNotice("ofxQNXSoundStream") << "setInput(ofBaseSoundOutput)";
	soundOutputPtr = soundOutput;
}

//--------------------------------------------------------------
bool ofxQNXSoundStream::setup(int outChannels, int inChannels, int _sampleRate, int _bufferSize, int nBuffers) {
	ofLogNotice("ofxQNXSoundStream") <<	"setup: outChannels " << outChannels <<	", inChannels " << inChannels << ", sampleRate " << _sampleRate << ", bufferSize " << _bufferSize << ", nBuffers " << nBuffers;

	nInputChannels = inChannels;
	nOutputChannels = outChannels;
	tickCount = 0;
	sampleRate = _sampleRate;
	bufferSize = _bufferSize;

	// not used.
	nBuffers = 1;

	// Setup QNX
	openQNXAudio();

	if (isRunning) {
		stop();
		close();
	}

	isSetup = true;
	ofSoundStreamStart();

	return true;
}

//--------------------------------------------------------------
bool ofxQNXSoundStream::setup(ofBaseApp * app, int outChannels, int inChannels, int sampleRate, int bufferSize, int nBuffers) {
	ofLogNotice("ofxQNXSoundStream") << "setup(ofBaseApp)";
	setInput(app);
	setOutput(app);

	return setup(outChannels, inChannels, sampleRate, bufferSize, nBuffers);
}

//--------------------------------------------------------------
void ofxQNXSoundStream::start() {
	ofLogNotice("ofxQNXSoundStream") << "start()";
	// TODO
	if (isRunning) {
		ofSoundStreamStop();
	}
}

//--------------------------------------------------------------
void ofxQNXSoundStream::stop() {
	ofLogNotice("ofxQNXSoundStream") << "stop()";
	// TODO
}

//--------------------------------------------------------------
void ofxQNXSoundStream::close() {
	ofLogNotice("ofxQNXSoundStream") << "close()";
	// TODO
}

//--------------------------------------------------------------
long unsigned long ofxQNXSoundStream::getTickCount() {
	return tickCount;
}

//--------------------------------------------------------------
int ofxQNXSoundStream::getNumInputChannels() {
	return nInputChannels;
}

//--------------------------------------------------------------
int ofxQNXSoundStream::getNumOutputChannels() {
	return nOutputChannels;
}
