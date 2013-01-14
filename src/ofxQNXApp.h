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

#ifndef OFXQNXAPP_H_
#define OFXQNXAPP_H_

#include "ofBaseApp.h"
#include "ofEvents.h"
#include "ofConstants.h"

class ofxQNXApp : public ofBaseApp
{
	public:
		virtual void touchDown(int x, int y, int id) {};
		virtual void touchMoved(int x, int y, int id) {};
		virtual void touchUp(int x, int y, int id) {};
		virtual void touchDoubleTap(int x, int y, int id) {};
		virtual void touchCancelled(int x, int y, int id) {};

		virtual void touchDown(ofTouchEventArgs & touch) {
			touchDown(touch.x, touch.y, touch.id);
		};
		virtual void touchMoved(ofTouchEventArgs & touch) {
			touchMoved(touch.x, touch.y, touch.id);
		};
		virtual void touchUp(ofTouchEventArgs & touch) {
			touchUp(touch.x, touch.y, touch.id);
		};
		virtual void touchDoubleTap(ofTouchEventArgs & touch) {
			touchDoubleTap(touch.x, touch.y, touch.id);
		};
		virtual void touchCancelled(ofTouchEventArgs & touch){
			touchCancelled(touch.x, touch.y, touch.id);
		}

		virtual void navigatorSwipeDown() {};
		virtual void navigatorWindowState(int state) {};
		virtual void navigatorWindowActive() {};
		virtual void navigatorWindowInactive() {};
};

#endif /* OFXQNXAPP_H_ */
