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

#include "ofAppQNXWindow.h"

#include "ofLog.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "ofUtils.h"

#include "ofxAccelerometer.h"
#include <bps/sensor.h>
#include <AL/alut.h>

#define DOUBLE_TAP_RANGE 10
#define DOUBLE_TAP_TIME 300

static ofxQNXApp *qnxApp;

//--------------------------------------------------------------
ofAppQNXWindow::ofAppQNXWindow() {
	ofLogNotice("ofAppQNXWindow") << "ofAppQNXWindow()";

	qnxFrameCount = 0;
	qnxFrameRate = 60.0f;
	qnxLastFrameTime = 0;

	qnxOrientation = OF_ORIENTATION_DEFAULT;

// Window size will automatically be set
	qnxWindowWidth = 1024;
	qnxWindowHeight = 600;

	qnxFrameRateSet = false;

	qnxSetupScreen = true;

	qnxIsRunning = true;
	qnxIsPaused = false;

	qnxMousePressed = false;
}

//--------------------------------------------------------------
ofAppQNXWindow::~ofAppQNXWindow() {
	ofLogNotice("ofAppQNXWindow") << "~ofAppQNXWindow()";
}

//--------------------------------------------------------------
void ofAppQNXWindow::setupOpenGL(int w, int h, int screenMode) {
	ofLogNotice("ofAppQNXWindow") << "setupOpenGL()";

	if(qnxInitialize() == 0) {
		exit(0);
	}
}

//--------------------------------------------------------------
void ofAppQNXWindow::runAppViaInfiniteLoop(ofBaseApp *appPtr) {
	ofLogNotice("ofAppQNXWindow") << "runAppViaInfiniteLoop()";

	ofNotifySetup();
	ofNotifyUpdate();

	qnxApp = dynamic_cast<ofxQNXApp*>(appPtr);
	if(qnxApp) {
		ofLogNotice("ofAppQNXWindow") << "ofRegisterTouchEvents";
		ofRegisterTouchEvents(qnxApp);
	}

	qnxMainLoop();
}

//--------------------------------------------------------------
int ofAppQNXWindow::getFrameNum() {
	return qnxFrameCount;
}

//--------------------------------------------------------------
float ofAppQNXWindow::getFrameRate() {
	return qnxFrameRate;
}

//--------------------------------------------------------------
double ofAppQNXWindow::getLastFrameTime() {
	return qnxLastFrameTime;
}

//--------------------------------------------------------------
ofPoint	ofAppQNXWindow::getWindowSize() {
	return ofPoint(qnxWindowWidth, qnxWindowHeight);
}

//--------------------------------------------------------------
void ofAppQNXWindow::setOrientation(ofOrientation orientation) {
	qnxOrientation = orientation;
}

//--------------------------------------------------------------
ofOrientation ofAppQNXWindow::getOrientation() {
	return qnxOrientation;
}

//--------------------------------------------------------------
int	ofAppQNXWindow::getWidth() {
	return qnxWindowWidth;
}

//--------------------------------------------------------------
int	ofAppQNXWindow::getHeight() {
	return qnxWindowHeight;
}

//--------------------------------------------------------------
void ofAppQNXWindow::setFrameRate(float targetRate) {
	qnxFrameRateSet = true;

	qnxRate = 1000 / targetRate;
	qnxSkip = 10 * qnxRate;
	qnxLastTime = 0;
	qnxCurrentTime = 0;
	qnxDelta = 0;
}

//--------------------------------------------------------------
void ofAppQNXWindow::enableSetupScreen() {
	qnxSetupScreen = true;
}

//--------------------------------------------------------------
void ofAppQNXWindow::disableSetupScreen() {
	qnxSetupScreen = false;
}

//--------------------------------------------------------------
int ofAppQNXWindow::qnxInitialize() {
	ofLogNotice("ofAppQNXWindow") << "qnxInitialize()";

	// Create a screen context that will be used to create an EGL surface to receive libscreen events
	screen_create_context(&qnxScreenContext, 0);

	// Initialize BPS library
	bps_initialize();

	// Use utility code to initialize EGL for rendering with GL ES 1.1
	if (EXIT_SUCCESS != bbutil_init_egl(qnxScreenContext)) {
		ofLogFatalError("ofAppQNXWindow") << "bbutil_init_egl failed";
		bbutil_terminate();
		screen_destroy_context(qnxScreenContext);
		return 0;
	}

	// Request screen events
	if (BPS_SUCCESS != screen_request_events(qnxScreenContext)) {
		ofLogFatalError("ofAppQNXWindow") << "screen_request_events failed";
		bbutil_terminate();
		screen_destroy_context(qnxScreenContext);
		return 0;
	}

	// Request navigator events
	if (BPS_SUCCESS != navigator_request_events(0)) {
		ofLogFatalError("ofAppQNXWindow") << "navigator_request_events failed";
		bbutil_terminate();
		screen_destroy_context(qnxScreenContext);
		return 0;
	}

	// Request virtual keyboard events
    if (BPS_SUCCESS != virtualkeyboard_request_events(0)) {
    	ofLogFatalError("ofAppQNXWindow") << "navigator_request_events failed";
        bbutil_terminate();
        screen_destroy_context(qnxScreenContext);
        return 0;
    }

	// Signal BPS library that navigator orientation is not to be locked
	if (BPS_SUCCESS != navigator_rotation_lock(false)) {
		ofLogFatalError("ofAppQNXWindow") << "navigator_rotation_lock failed";
		bbutil_terminate();
		screen_destroy_context(qnxScreenContext);
		return 0;
	}

	// Get screen dimensions
	EGLint surface_width, surface_height;
	eglQuerySurface(egl_disp, egl_surf, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_disp, egl_surf, EGL_HEIGHT, &surface_height);

	EGLint err = eglGetError();
	if (err != 0x3000) {
		ofLogError("ofAppQNXWindow") << "Unable to query EGL surface dimensions";
		return 0;
	}

	ofLogNotice("ofAppQNXWindow") << "surface_width: " << surface_width;
	ofLogNotice("ofAppQNXWindow") << "surface_height: " << surface_height;

	qnxWindowWidth = surface_width;
	qnxWindowHeight = surface_height;

	// Setup OpenAL
	if(!alutInit(0, 0)) {
		ofLogFatalError("ofAppQNXWindow") << "Could not start sound: " << alutGetError();
		return(0);
	}

	return 1;
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxQuit() {
	ofLogNotice("ofAppQNXWindow") << "qnxQuit()";

	// Stop OpenAL
	alutExit();

	// Stop requesting events from libscreen
	screen_stop_events(qnxScreenContext);

	// Shut down BPS library for this process
	bps_shutdown();

	// Use utility code to terminate EGL setup
	bbutil_terminate();

	// Destroy libscreen context
	screen_destroy_context(qnxScreenContext);
}

//--------------------------------------------------------------
int ofAppQNXWindow::qnxResize(bps_event_t *event) {

	//Query width and height of the window surface created by utility code
	EGLint surface_width, surface_height;

	if (event) {
		int angle = navigator_event_get_orientation_angle(event);

		//Let bbutil rotate current screen surface to this angle
		if (EXIT_FAILURE == bbutil_rotate_screen_surface(angle)) {
			ofLogError("ofAppQNXWindow") << "Unable to handle orientation change";
			return EXIT_FAILURE;
		}
	}

	eglQuerySurface(egl_disp, egl_surf, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_disp, egl_surf, EGL_HEIGHT, &surface_height);

	EGLint err = eglGetError();
	if (err != 0x3000) {
		ofLogError("ofAppQNXWindow") << "Unable to query EGL surface dimensions";
		return EXIT_FAILURE;
	}

	qnxWindowWidth = surface_width;
	qnxWindowHeight = surface_height;

	ofNotifyWindowResized(qnxWindowWidth, qnxWindowHeight);

	qnxUpdate();
	if (event) {
		qnxRender();
		navigator_done_orientation(event);
	}

	return EXIT_SUCCESS;
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxMainLoop() {
	ofLogNotice("ofAppQNXWindow") << "qnxMainLoop()";

	while(qnxIsRunning) {
		qnxHandleEvents();

		if(!qnxIsPaused) {
			if(!qnxFrameRateSet) {
				// Update and Render in sync and as fast as it can.
				qnxUpdate();
				qnxRender();
			}
			else {
				// Update and Render async, update will be called at a fixed rate
				qnxCurrentTime = ofGetElapsedTimeMillis();

				if (qnxLastTime < 0) {
					qnxLastTime = qnxCurrentTime;
				}

				qnxDelta += qnxCurrentTime - qnxLastTime;
				qnxLastTime = qnxCurrentTime;

				if (qnxDelta >= qnxRate) {
					while(qnxDelta > qnxSkip) {
						qnxDelta -= qnxSkip;
					}

					while (qnxDelta >= qnxRate)	{
						qnxDelta -= qnxRate;
						qnxUpdate();
					}

					qnxRender();
					qnxHasRest = false;
				}
				else {
					if(!qnxHasRest) {
						usleep((qnxRate - qnxDelta) * 1000);
					}
					qnxHasRest = true;
				}
			}
		}
	}

	qnxQuit();
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxHandleEvents() {
	// Handle all BB events
	bps_event_t *event_bps;
	int event_domain;
	int	event_timeout;
	int	rc = 0;

	for(;;) {
		if(qnxIsPaused) {
			event_timeout = -1;	// block until some events comes in
		}
		else {
			event_timeout = 0;	// return immediately
		}

		event_bps = NULL;
		rc = bps_get_event(&event_bps, event_timeout);
		assert(rc == BPS_SUCCESS);

		if (event_bps != NULL) {
			event_domain = bps_event_get_domain(event_bps);

			if (event_domain == screen_get_domain()) {
				qnxHandleScreenEvent(event_bps);
			}
			else if (event_domain == navigator_get_domain()) {
				switch (bps_event_get_code(event_bps)) {

					case NAVIGATOR_ORIENTATION_CHECK: {
						ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_ORIENTATION_CHECK";
				        //Signal navigator that we intend to resize
				        navigator_orientation_check_response(event_bps, true);
					}
					break;

				    case NAVIGATOR_ORIENTATION: {
				    	ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_ORIENTATION";
				        if (EXIT_FAILURE == qnxResize(event_bps)) {
				            ofLogError("ofAppQNXWindow") << "Could not resize...";
							qnxIsRunning = false;
							sensor_stop_events(SENSOR_TYPE_ACCELEROMETER);
							return;
				        }
				    }
				    break;

					case NAVIGATOR_SWIPE_DOWN: {
						ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_SWIPE_DOWN";
						qnxApp->navigatorSwipeDown();
					}
					break;

					case NAVIGATOR_WINDOW_STATE: {
						switch (navigator_event_get_window_state(event_bps))
						{
							case NAVIGATOR_WINDOW_FULLSCREEN: {
								ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_WINDOW_FULLSCREEN";
								qnxApp->navigatorWindowState(NAVIGATOR_WINDOW_FULLSCREEN);
								qnxIsPaused = false;
							}
							break;

							case NAVIGATOR_WINDOW_THUMBNAIL: {
								ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_WINDOW_THUMBNAIL";
								qnxApp->navigatorWindowState(NAVIGATOR_WINDOW_THUMBNAIL);
								qnxIsPaused = true;
							}
							break;

							case NAVIGATOR_WINDOW_INVISIBLE: {
								ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_WINDOW_INVISIBLE";
								qnxApp->navigatorWindowState(NAVIGATOR_WINDOW_INVISIBLE);
								qnxIsPaused = true;
							}
							break;
						}
					}
					break;

					case NAVIGATOR_WINDOW_ACTIVE: {
						ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_WINDOW_ACTIVE";
						qnxApp->navigatorWindowActive();
					}
					break;

					case NAVIGATOR_WINDOW_INACTIVE: {
						ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_WINDOW_INACTIVE";
						qnxApp->navigatorWindowInactive();
					}
					break;

					case NAVIGATOR_EXIT: {
						ofLogNotice("ofAppQNXWindow") << "NAVIGATOR_EXIT";

						qnxIsRunning = false;
						sensor_stop_events(SENSOR_TYPE_ACCELEROMETER);
						return;
					}
					break;
				}
			}
			else if (bps_event_get_domain(event_bps) == sensor_get_domain()) {
				if (SENSOR_ACCELEROMETER_READING == bps_event_get_code(event_bps)) {
					sensor_event_get_xyz(event_bps, &qnxForceX, &qnxForceY, &qnxForceZ);
					ofxAccelerometer.update(-qnxForceX, -qnxForceY, -qnxForceZ);
				}
			}
		}
		else {
			// No events, continue.
			break;
		}
	}
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxUpdate() {
	// Update application logic
	ofNotifyUpdate();
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxRender() {
	// Render results
	unsigned long beginFrameMicros = ofGetElapsedTimeMicros();

	// Store last frame time
	qnxLastFrameTime = double(beginFrameMicros - qnxPreviousFrameMicros)/1000000.;
	qnxPreviousFrameMicros = beginFrameMicros;

	// Setup viewport
	ofViewport(0, 0, qnxWindowWidth, qnxWindowHeight, false);

	// Clear background
	bool bClearAuto = ofbClearBg();
	if(bClearAuto == true || qnxFrameCount < 3) {
		float *bgPtr = ofBgColorPtr();
		ofClear(bgPtr[0]*255, bgPtr[1]*255, bgPtr[2]*255, bgPtr[3]*255);
	}

	// Setup screen if needed
	if(qnxSetupScreen) {
		ofSetupScreen();
	}

	// Draw
	ofNotifyDraw();
	++qnxFrameCount;

	// Update framerate value
	++qnxFrames;
	if(beginFrameMicros - qnxOneSecond >= 1000000) {
		qnxFrameRate = qnxFrames;
		qnxFrames = 0;
		qnxOneSecond = beginFrameMicros;
	}

	// Swap buffers
	bbutil_swap();
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxHandleScreenEvent(bps_event_t *event) {
    screen_event_t screen_event = screen_event_get_event(event);
    mtouch_event_t  mtouch_event;
    int touch_id;

    int screen_val;
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);

    if (screen_val == SCREEN_EVENT_MTOUCH_TOUCH) {
    	// Handle an initial finger press event here
    	screen_get_mtouch_event(screen_event, &mtouch_event, 0);
    	touch_id = mtouch_event.contact_id;
    	qnxOnTouchDown(touch_id, mtouch_event.x, mtouch_event.y, mtouch_event.pressure);
	}
    else if (screen_val == SCREEN_EVENT_MTOUCH_MOVE) {
		// Handle a finger move event here
    	screen_get_mtouch_event(screen_event, &mtouch_event, 0);
		touch_id = mtouch_event.contact_id;
		qnxOnTouchMove(touch_id, mtouch_event.x, mtouch_event.y, mtouch_event.pressure);
	}
    else if (screen_val == SCREEN_EVENT_MTOUCH_RELEASE) {
		// Handle a finger release event here
    	screen_get_mtouch_event(screen_event, &mtouch_event, 0);
		touch_id = mtouch_event.contact_id;
		qnxOnTouchUp(touch_id, mtouch_event.x, mtouch_event.y, mtouch_event.pressure);
	}
    else if (screen_val == SCREEN_EVENT_KEYBOARD) {
    	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_FLAGS, &screen_val);

		if (screen_val & KEY_DOWN) {
			screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_SYM,&screen_val);

			//printf("The '%c' (%d) key was pressed\n", (char)screen_val, screen_val);
			//fflush(stdout);

			// Special cases?
			if (screen_val == 61453) 				// return
				ofNotifyKeyPressed(OF_KEY_RETURN);
			else if (screen_val == 13) 				// return BB10
				ofNotifyKeyPressed(OF_KEY_RETURN);
			else if (screen_val == 61448) 			// backspace
				ofNotifyKeyPressed(OF_KEY_BACKSPACE);
			else if (screen_val == 8) 			// backspace BB10
				ofNotifyKeyPressed(OF_KEY_BACKSPACE);
			else if (screen_val == 8220) 			// left double quotation mark
				return; //ofNotifyKeyPressed((int)"“");
			else if (screen_val == 8221) 			// right double quotation mark
				return; //ofNotifyKeyPressed((int)"”");
			else
				ofNotifyKeyPressed(screen_val);
		}
		else {
			screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_SYM,&screen_val);

			//printf("The '%c' key was released\n", (char)screen_val);
			//fflush(stdout);

			// Special cases?
			if (screen_val == 61453) 				// return
				ofNotifyKeyReleased(OF_KEY_RETURN);
			else if (screen_val == 13) 				// return BB10
				ofNotifyKeyReleased(OF_KEY_RETURN);
			else if (screen_val == 61448) 			// backspace
				ofNotifyKeyReleased(OF_KEY_BACKSPACE);
			else if (screen_val == 8) 			// backspace BB10
				ofNotifyKeyReleased(OF_KEY_BACKSPACE);
			else if (screen_val == 8220) 			// left double quotation mark
				return; //ofNotifyKeyReleased((int)"“");
			else if (screen_val == 8221) 			// right double quotation mark
				return; //ofNotifyKeyReleased((int)"”");			
			else
				ofNotifyKeyReleased(screen_val);
		}
    }
    else if (screen_val == SCREEN_EVENT_POINTER) {
		// Handle a mouse event here (simulator only)
		int buttons;
		int pair[2];
		screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_BUTTONS, &buttons);
		screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, pair);

		if(!qnxMousePressed) {
			if(buttons == 1) {
				qnxOnTouchDown(0, pair[0], pair[1], 0);
				qnxMousePressed = true;
			}
		}
		else {
			if(buttons == 1) {
				qnxOnTouchMove(0, pair[0], pair[1], 0);
			}
			else if(buttons == 0) {
				qnxOnTouchUp(0, pair[0], pair[1], 0);
				qnxMousePressed = false;
			}
		}
	}
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxOnTouchDown(unsigned int id, int x, int y, int pressure) {
	ofNotifyMousePressed(x, y, 0);

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
		for (unsigned int i = 0; i < qnxTouchList.size(); ++i) {
			if(abs(qnxTouchList[i].x - touch.x) < DOUBLE_TAP_RANGE && abs(qnxTouchList[i].y - touch.y) < DOUBLE_TAP_RANGE && (touch.time - qnxTouchList[i].time) < DOUBLE_TAP_TIME)	{
				found = i;
				qnxTouchList.erase (qnxTouchList.begin() + found);
				break;
			}
		}

		// Not found, add touch to history
		if(found == -1) {
			qnxTouchList.push_back (touch);
		}		else {
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
		if(!qnxTouchList.empty()) {
			for (int j = qnxTouchList.size() - 1; j >= 0; --j) {
				if((touch.time - qnxTouchList[j].time) > DOUBLE_TAP_TIME) {
					qnxTouchList.erase (qnxTouchList.begin() + j);
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxOnTouchMove(unsigned int id, int x, int y, int pressure) {
	ofNotifyMouseMoved(x, y);
	ofNotifyMouseDragged(x, y, 0);

	ofTouchEventArgs touch;
	touch.id = id;
	touch.x = x;
	touch.y = y;
	touch.pressure = pressure;
	touch.type = ofTouchEventArgs::move;
	touch.time = ofGetElapsedTimeMillis();
	ofNotifyEvent(ofEvents().touchMoved, touch);
}

//--------------------------------------------------------------
void ofAppQNXWindow::qnxOnTouchUp(unsigned int id, int x, int y, int pressure) {
	ofNotifyMouseReleased(x, y, 0);

	ofTouchEventArgs touch;
	touch.id = id;
	touch.x = x;
	touch.y = y;
	touch.pressure = pressure;
	touch.type = ofTouchEventArgs::up;
	touch.time = ofGetElapsedTimeMillis();
	ofNotifyEvent(ofEvents().touchUp, touch);
}
