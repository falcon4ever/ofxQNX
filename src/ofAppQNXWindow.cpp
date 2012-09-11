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
Notes:
- based on ofxAndroid and ofxiPhone, still need to clean up the source and implement missing stuff
*/

#include "ofAppQNXWindow.h"

#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "ofUtils.h"
#include "ofxQNXApp.h"

#include "ofxAccelerometer.h"
#include <bps/sensor.h>

#define DOUBLE_TAP_RANGE 10
#define DOUBLE_TAP_TIME 300

static ofxQNXApp * qnxApp;

ofAppQNXWindow::ofAppQNXWindow()
{
	fprintf(stderr, "ofAppQNXWindow::ofAppQNXWindow()\n");
	ofLog(OF_LOG_VERBOSE, "ofAppQNXWindow::ofAppQNXWindow()");

	exit_application = 0;

	nFrameCount = 0;
	lastFrameTime = 0;
	frameRate = 60.0f;	
	orientation = OF_ORIENTATION_DEFAULT;
	bSetupScreen = true;

	previousFrameMicros = 0;
	frames = 0;
	onesec = 0;
	bFrameRateSet = false;
	oneFrameTime = 0;

	mousePressed = false;

	screenWidth = 1024;
	screenHeigth = 600;
}

ofAppQNXWindow::~ofAppQNXWindow()
{

}

void ofAppQNXWindow::onTouchDown(unsigned int id, int x, int y, int pressure)
{
	ofNotifyMousePressed(x,y,0);

	ofTouchEventArgs touch;
	touch.id = id;
	touch.x = x;
	touch.y = y;
	touch.pressure = pressure;
	touch.type = ofTouchEventArgs::down;
	touch.time = ofGetElapsedTimeMillis();
	ofNotifyEvent(ofEvents().touchDown, touch);

	// check for doubletap event
	{
		// Compare position and last touch
		int found = -1;
		for (unsigned int i = 0; i < touchList.size(); ++i)
		{
			if(	abs(touchList[i].x - touch.x) < DOUBLE_TAP_RANGE &&
				abs(touchList[i].y - touch.y) < DOUBLE_TAP_RANGE &&
				(touch.time - touchList[i].time) < DOUBLE_TAP_TIME
			  )
			{
				found = i;
				touchList.erase (touchList.begin() + found);
				break;
			}
		}

		// Not found, add touch to history
		if(found == -1)
		{
			touchList.push_back (touch);
		}		else
		{
			// Prepare to trigger doubleTap event
			ofTouchEventArgs touch2;
			touch2.id = touch.id;
			touch2.x = touch.x;
			touch2.y = touch.y;
			touch2.pressure = touch.pressure;
			touch2.type = ofTouchEventArgs::doubleTap;
			touch2.time = touch.time;
			ofNotifyEvent(ofEvents().touchDoubleTap, touch2);
		}

		// Clean up old touches
		if(!touchList.empty())
			for (int j = touchList.size() - 1; j >= 0; --j)
			{
				if((touch.time - touchList[j].time) > DOUBLE_TAP_TIME)
				{
					touchList.erase (touchList.begin() + j);
				}
			}
	}
}

void ofAppQNXWindow::onTouchMove(unsigned int id, int x, int y, int pressure)
{
	ofNotifyMouseMoved(x,y);
	ofNotifyMouseDragged(x,y,0);

	ofTouchEventArgs touch;
	touch.id = id;
	touch.x = x;
	touch.y = y;
	touch.pressure = pressure;
	touch.type = ofTouchEventArgs::move;
	touch.time = ofGetElapsedTimeMillis();
	ofNotifyEvent(ofEvents().touchMoved, touch);
}

void ofAppQNXWindow::onTouchUp(unsigned int id, int x, int y, int pressure)
{
	ofNotifyMouseReleased(x,y,0);

	ofTouchEventArgs touch;
	touch.id = id;
	touch.x = x;
	touch.y = y;
	touch.pressure = pressure;
	touch.type = ofTouchEventArgs::up;
	touch.time = ofGetElapsedTimeMillis();
	ofNotifyEvent(ofEvents().touchUp, touch);
}

void ofAppQNXWindow::updateFrame()
{
	unsigned long beginFrameMicros = ofGetElapsedTimeMicros();

	lastFrameTime = double(beginFrameMicros - previousFrameMicros)/1000000.;
	previousFrameMicros = beginFrameMicros;

	ofNotifyUpdate();

	int width, height;
	width  = getWidth();
	height = getHeight();

	height = height > 0 ? height : 1;
	// set viewport, clear the screen
	//glViewport( 0, 0, width, height );
	ofViewport(0, 0, width, height, false);		// used to be glViewport( 0, 0, width, height );

	float * bgPtr = ofBgColorPtr();
	bool bClearAuto = ofbClearBg();

	if ( bClearAuto == true || nFrameCount < 3){
		ofClear(bgPtr[0]*255,bgPtr[1]*255,bgPtr[2]*255, bgPtr[3]*255);
	}

	if(bSetupScreen) ofSetupScreen();
	ofNotifyDraw();

	unsigned long currTime = ofGetElapsedTimeMicros();
	unsigned long frameMicros = currTime - beginFrameMicros;
	
	++nFrameCount;
	++frames;

	if(currTime - onesec>=1000000){
		frameRate = frames;
		frames = 0;
		onesec = currTime;
	}

	if(bFrameRateSet && frameMicros<oneFrameTime) usleep(oneFrameTime-frameMicros);

}

void ofAppQNXWindow::setupOpenGL(int w, int h, int screenMode)
{
	fprintf(stderr, "ofAppQNXWindow::setupOpenGL()\n");
	ofLog(OF_LOG_VERBOSE, "ofAppQNXWindow::setupOpenGL()");

	if(qnxInitialize() == 0)
		exit(0);
}

void ofAppQNXWindow::initializeWindow()
{
	fprintf(stderr, "ofAppQNXWindow::initializeWindow()\n");
	ofLog(OF_LOG_VERBOSE, "ofAppQNXWindow::initializeWindow()");
}

void ofAppQNXWindow::runAppViaInfiniteLoop(ofBaseApp * appPtr)
{
	fprintf(stderr, "ofAppQNXWindow::runAppViaInfiniteLoop()\n");
	ofLog(OF_LOG_VERBOSE, "ofAppQNXWindow::runAppViaInfiniteLoop()");

	ofNotifySetup();
	ofNotifyUpdate();
	qnxApp = dynamic_cast<ofxQNXApp*>( appPtr );

	runAppLoop();
}

int ofAppQNXWindow::getFrameNum()
{
	// TODO
	return nFrameCount;
}

float ofAppQNXWindow::getFrameRate()
{
	// TODO
	return frameRate;
}

double ofAppQNXWindow::getLastFrameTime()
{
	// TODO
	return lastFrameTime;
}


ofPoint	ofAppQNXWindow::getWindowSize()
{
	// TODO
	return ofPoint(getWidth(),getHeight());
}

	
void ofAppQNXWindow::setOrientation(ofOrientation _orientation)
{
	// TODO
	orientation = _orientation;
}

ofOrientation ofAppQNXWindow::getOrientation()
{
	// TODO
	return orientation;
}

int	ofAppQNXWindow::getWidth()
{
	return screenWidth;
}

int	ofAppQNXWindow::getHeight()
{
	return screenHeigth;
}

void ofAppQNXWindow::setFrameRate(float targetRate)
{
	frameRate = targetRate;
	oneFrameTime = 1000000.f/targetRate;
	bFrameRateSet = true;
}

void ofAppQNXWindow::setFullscreen(bool fullscreen)
{
	// TODO
	// Not used?
}

void ofAppQNXWindow::toggleFullscreen()
{
	// TODO
	// Not used?
}

void ofAppQNXWindow::enableSetupScreen()
{
	bSetupScreen = true;
}

void ofAppQNXWindow::disableSetupScreen()
{
	bSetupScreen = false;
}

void ofAppQNXWindow::runAppLoop()
{
	fprintf(stderr, "ofAppQNXWindow::runLoop()\n");

	while (!exit_application) {
		//Request and process all available BPS events
		bps_event_t *event = NULL;

		for(;;) {
			rc = bps_get_event(&event, 0);
			assert(rc == BPS_SUCCESS);

			if (event)
			{
				int domain = bps_event_get_domain(event);
				if (domain == screen_get_domain())
				{
					qnxHandleScreenEvent(event);
				}
				/*
				// Disabled keyboard, use this call manually to bring up the keyboard.
				else if ((domain == navigator_get_domain()) && (NAVIGATOR_SWIPE_DOWN == bps_event_get_code(event)))
				{
					virtualkeyboard_show();
				}
				*/
				else if ((domain == navigator_get_domain()) && (NAVIGATOR_EXIT == bps_event_get_code(event)))
				{
					exit_application = 1;
					sensor_stop_events(SENSOR_TYPE_ACCELEROMETER);
				}
				else if (bps_event_get_domain(event) == sensor_get_domain())
				{
					if (SENSOR_ACCELEROMETER_READING == bps_event_get_code(event))
					{
						/*
						 * Extract the accelerometer data from the event and
						 * display it.
						 */
						sensor_event_get_xyz(event, &force_x, &force_y, &force_z);
						//fprintf(stderr, "display_accelerometer_reading x%f y%f z%f\n", force_x, force_y, force_z);
						ofxAccelerometer.update(-force_x, -force_y, -force_z); // inverted axis?
					}
				}
			}
			else
			{
				break;
			}
		}

		updateFrame();
		bbutil_swap();
	}

	qnxQuit();
}

void ofAppQNXWindow::qnxHandleScreenEvent(bps_event_t *event)
{
    screen_event_t screen_event = screen_event_get_event(event);
    mtouch_event_t  mtouch_event;
    int touch_id;

    int screen_val;
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);

    if (screen_val == SCREEN_EVENT_MTOUCH_TOUCH)
    {
    	// Handle an initial finger press event here
    	screen_get_mtouch_event(screen_event, &mtouch_event, 0);
    	touch_id = mtouch_event.contact_id;
    	onTouchDown(touch_id, mtouch_event.x, mtouch_event.y, mtouch_event.pressure);
	}
    else if (screen_val == SCREEN_EVENT_MTOUCH_MOVE)
    {
		// Handle a finger move event here
    	screen_get_mtouch_event(screen_event, &mtouch_event, 0);
		touch_id = mtouch_event.contact_id;
		onTouchMove(touch_id, mtouch_event.x, mtouch_event.y, mtouch_event.pressure);
	}
    else if (screen_val == SCREEN_EVENT_MTOUCH_RELEASE)
    {
		// Handle a finger release event here
    	screen_get_mtouch_event(screen_event, &mtouch_event, 0);
		touch_id = mtouch_event.contact_id;
		onTouchUp(touch_id, mtouch_event.x, mtouch_event.y, mtouch_event.pressure);
	}
    else if (screen_val == SCREEN_EVENT_KEYBOARD)
    {
    	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_FLAGS, &screen_val);

		if (screen_val & KEY_DOWN)
		{
			screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_SYM,&screen_val);

			//printf("The '%c' (%d) key was pressed\n", (char)screen_val, screen_val);
			//fflush(stdout);

			// Special cases?
			if (screen_val == 61453) 				// return
				ofNotifyKeyPressed(OF_KEY_RETURN);
			else if (screen_val == 61448) 			// backspace
				ofNotifyKeyPressed(OF_KEY_BACKSPACE);
			else if (screen_val == 8220) 			// left double quotation mark
				return; //ofNotifyKeyPressed((int)"“");
			else if (screen_val == 8221) 			// right double quotation mark
				return; //ofNotifyKeyPressed((int)"”");
			else
				ofNotifyKeyPressed(screen_val);
		}
		else
		{
			screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_SYM,&screen_val);

			//printf("The '%c' key was released\n", (char)screen_val);
			//fflush(stdout);

			// Special cases?
			if (screen_val == 61453) 				// return
				ofNotifyKeyReleased(OF_KEY_RETURN);
			else if (screen_val == 61448) 			// backspace
				ofNotifyKeyReleased(OF_KEY_BACKSPACE);
			else if (screen_val == 8220) 			// left double quotation mark
				return; //ofNotifyKeyPressed((int)"“");
			else if (screen_val == 8221) 			// right double quotation mark
				return; //ofNotifyKeyPressed((int)"”");
			else
				ofNotifyKeyReleased(screen_val);
		}
    }
    else if (screen_val == SCREEN_EVENT_POINTER)
	{
		// Handle a mouse event here (simulator only)
		int buttons;
		int pair[2];
		screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_BUTTONS, &buttons);
		screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, pair);

		if(!mousePressed)
		{
			if(buttons == 1)
			{
				onTouchDown(0, pair[0], pair[1], 0);
				mousePressed = true;
			}
		}
		else
		{
			if(buttons == 1)
			{
				onTouchMove(0, pair[0], pair[1], 0);
			}
			else if(buttons == 0)
			{
				onTouchUp(0, pair[0], pair[1], 0);
				mousePressed = false;
			}
		}
	}
}

/*
//Query width and height of the window surface created by utility code
EGLint surface_width, surface_height;

eglQuerySurface(egl_disp, egl_surf, EGL_WIDTH, &surface_width);
eglQuerySurface(egl_disp, egl_surf, EGL_HEIGHT, &surface_height);

EGLint err = eglGetError();
if (err != 0x3000) {
	fprintf(stderr, "Unable to query EGL surface dimensions\n");
	return EXIT_FAILURE;
}
*/

int ofAppQNXWindow::qnxInitialize()
{
	//Create a screen context that will be used to create an EGL surface to to receive libscreen events
	screen_create_context(&screen_cxt, 0);

	//Initialize BPS library
	bps_initialize();

	//Use utility code to initialize EGL for rendering with GL ES 1.1
	if (EXIT_SUCCESS != bbutil_init_egl(screen_cxt)) {
		fprintf(stderr, "bbutil_init_egl failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	/*
	//Initialize application logic
	if (EXIT_SUCCESS != initialize()) {
		fprintf(stderr, "initialize failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}
	*/

	//Signal BPS library that navigator and screen events will be requested
	if (BPS_SUCCESS != screen_request_events(screen_cxt)) {
		fprintf(stderr, "screen_request_events failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	if (BPS_SUCCESS != navigator_request_events(0)) {
		fprintf(stderr, "navigator_request_events failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

    if (BPS_SUCCESS != virtualkeyboard_request_events(0)) {
        fprintf(stderr, "navigator_request_events failed\n");
        bbutil_terminate();
        screen_destroy_context(screen_cxt);
        return 0;
    }

	//Signal BPS library that navigator orientation is not to be locked
	if (BPS_SUCCESS != navigator_rotation_lock(false)) {
		fprintf(stderr, "navigator_rotation_lock failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}


	// Get screen dimensions
	int count = 0;
	screen_get_context_property_iv(screen_cxt, SCREEN_PROPERTY_DISPLAY_COUNT, &count);
	screen_display_t *screen_disps = (screen_display_t *)calloc(count, sizeof(screen_display_t));
	screen_get_context_property_pv(screen_cxt, SCREEN_PROPERTY_DISPLAYS, (void **)screen_disps);

	screen_display_t screen_disp = screen_disps[0];
	free(screen_disps);

	int dims[2] = { 0, 0 };
	screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_SIZE, dims);

	screenWidth = dims[0];
	screenHeigth = dims[1];

	return 1;
}

void ofAppQNXWindow::qnxQuit()
{
	//Stop requesting events from libscreen
	screen_stop_events(screen_cxt);

	//Shut down BPS library for this process
	bps_shutdown();

	//Use utility code to terminate EGL setup
	bbutil_terminate();

	//Destroy libscreen context
	screen_destroy_context(screen_cxt);
}
