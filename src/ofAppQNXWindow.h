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

#ifndef OFAPPQNXWINDOW_H_
#define OFAPPQNXWINDOW_H_

#include "ofAppBaseWindow.h"

#include <assert.h>
#include <screen/screen.h>
#include <input/screen_helpers.h>

#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>
#include "bbutil.h"
#include <bps/virtualkeyboard.h>
#include <sys/keycodes.h>

#include "ofEvents.h"

class ofAppQNXWindow : public ofAppBaseWindow 
{
	public:
		ofAppQNXWindow();
		virtual ~ofAppQNXWindow();

		void onTouchDown(unsigned int id, int x, int y, int pressure);
		void onTouchMove(unsigned int id, int x, int y, int pressure);
		void onTouchUp(unsigned int id, int x, int y, int pressure);
		void updateFrame();
		
		void setupOpenGL(int w, int h, int screenMode);
		void initializeWindow();
		void runAppViaInfiniteLoop(ofBaseApp * appPtr);

		void hideCursor() {}
		void showCursor() {}

		void	setWindowPosition(int x, int y) {}
		void	setWindowShape(int w, int h) {}

		int		getFrameNum();
		float	getFrameRate();
		double  getLastFrameTime();

		ofPoint	getWindowPosition() { return ofPoint(); }
		ofPoint	getWindowSize();
		ofPoint	getScreenSize() { return getWindowSize(); }
		
		void			setOrientation(ofOrientation orientation);
		ofOrientation	getOrientation();
		bool	doesHWOrientation() { return true; }


		int		getWidth();
		int		getHeight();

		void	setFrameRate(float targetRate);
		void	setWindowTitle(string title){}

		int		getWindowMode() {return 0;}

		void	setFullscreen(bool fullscreen);
		void	toggleFullscreen();

		void	enableSetupScreen();
		void	disableSetupScreen();
		
	protected:
		void    runAppLoop();

		void    qnxHandleScreenEvent(bps_event_t *event);
		int     qnxInitialize();
		void    qnxQuit();

		int rc;
		int exit_application;
		screen_context_t screen_cxt;

		std::vector<ofTouchEventArgs> touchList;

		float force_x, force_y, force_z;

		bool mousePressed;

		int		nFrameCount;
		float	frameRate;
		double	lastFrameTime;
		ofOrientation orientation;
		bool	bSetupScreen;

		unsigned long previousFrameMicros;
		int frames;
		unsigned long onesec;
		bool bFrameRateSet;
		unsigned long oneFrameTime;

		int screenWidth;
		int screenHeigth;
};

#endif /* OFAPPQNXWINDOW_H_ */
