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

#include "ofxQNXLogChannel.h"

//--------------------------------------------------------------
ofxQNXLogChannel::ofxQNXLogChannel() {

}

//--------------------------------------------------------------
void ofxQNXLogChannel::log(ofLogLevel level, const string & module, const string& msg) {
	string logLevelString;

	switch (level) {
		case OF_LOG_VERBOSE:
			logLevelString = "verbose";
			break;
		case OF_LOG_NOTICE:
			logLevelString = "notice";
			break;
		case OF_LOG_WARNING:
			logLevelString = "warning";
			break;
		case OF_LOG_ERROR:
			logLevelString = "error";
			break;
		case OF_LOG_FATAL_ERROR:
			logLevelString = "fatal error";
			break;
		default:
			logLevelString = "verbose";
			break;
	}

	if(module != "OF") {
		fprintf(stderr, "[%s:%s] %s\n", module.c_str(), logLevelString.c_str(), msg.c_str());
	}
	else {
		fprintf(stderr, "[%s] %s\n", logLevelString.c_str(), msg.c_str());
	}
}

//--------------------------------------------------------------
void ofxQNXLogChannel::log(ofLogLevel logLevel, const string & module, const char* format, ...) {
	va_list args;
	va_start(args, format);
	log(logLevel, module, format, args);
	va_end(args);
}

//--------------------------------------------------------------
void ofxQNXLogChannel::log(ofLogLevel logLevel, const string & module, const char* format, va_list args) {
	string logLevelString;

	switch (logLevel) {
		case OF_LOG_VERBOSE:
			logLevelString = "verbose";
			break;
		case OF_LOG_NOTICE:
			logLevelString = "notice";
			break;
		case OF_LOG_WARNING:
			logLevelString = "warning";
			break;
		case OF_LOG_ERROR:
			logLevelString = "error";
			break;
		case OF_LOG_FATAL_ERROR:
			logLevelString = "fatal error";
			break;
		default:
			logLevelString = "verbose";
			break;
	}

	fprintf(stderr, "[");
	if(module != "OF") {
		fprintf(stderr, "%s:", module.c_str());
	}
	fprintf(stderr, "%s] ", logLevelString.c_str());
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
}
