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
*/

#include "ofxQNXSoundPlayer.h"
#include "ofLog.h"

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::loadSound(string fileName, bool stream) {

	ofLog(OF_LOG_WARNING, "Method not implemented (loadSound)");
	return false;
};

//--------------------------------------------------------------
void ofxQNXSoundPlayer::unloadSound() {
	ofLog(OF_LOG_WARNING, "Method not implemented (unloadSound)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::play() {
	ofLog(OF_LOG_WARNING, "Method not implemented (play)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::stop() {
	ofLog(OF_LOG_WARNING, "Method not implemented (stop)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setVolume(float vol) {
	ofLog(OF_LOG_WARNING, "Method not implemented (setVolume)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPan(float vol) {
	ofLog(OF_LOG_WARNING, "Method not implemented (setPan)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setSpeed(float spd) {
	ofLog(OF_LOG_WARNING, "Method not implemented (setSpeed)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPaused(bool bP) {
	ofLog(OF_LOG_WARNING, "Method not implemented (setPaused)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setLoop(bool bLp) {
	ofLog(OF_LOG_WARNING, "Method not implemented (setLoop)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setMultiPlay(bool bMp) {
	ofLog(OF_LOG_WARNING, "Method not implemented (setMultiPlay)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPosition(float pct) {
	// 0 = start, 1 = end;
	ofLog(OF_LOG_WARNING, "Method not implemented (setPosition)");
}

//--------------------------------------------------------------
void ofxQNXSoundPlayer::setPositionMS(int ms) {
	ofLog(OF_LOG_WARNING, "Method not implemented (setPositionMS)");
}

//--------------------------------------------------------------
float ofxQNXSoundPlayer::getPosition() {
	ofLog(OF_LOG_WARNING, "Method not implemented (getPosition)");
	return 0;
}

//--------------------------------------------------------------
int ofxQNXSoundPlayer::getPositionMS() {
	ofLog(OF_LOG_WARNING, "Method not implemented (getPositionMS)");
	return 0;
}

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::getIsPlaying() {
	ofLog(OF_LOG_WARNING, "Method not implemented (getIsPlaying)");
	return false;
}

//--------------------------------------------------------------
float ofxQNXSoundPlayer::getSpeed() {
	ofLog(OF_LOG_WARNING, "Method not implemented (getSpeed)");
	return 0;
}

//--------------------------------------------------------------
float ofxQNXSoundPlayer::getPan() {
	ofLog(OF_LOG_WARNING, "Method not implemented (getPan)");
	return 0;
}

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::getIsPaused() {
	ofLog(OF_LOG_WARNING, "Method not implemented (getIsPaused)");
	return false;
}

//--------------------------------------------------------------
bool ofxQNXSoundPlayer::isLoaded() {
	ofLog(OF_LOG_WARNING, "Method not implemented (isLoaded)");
	return false;
}

//--------------------------------------------------------------
float ofxQNXSoundPlayer::getVolume() {
	ofLog(OF_LOG_WARNING, "Method not implemented (getVolume)");
	return 0;
}

