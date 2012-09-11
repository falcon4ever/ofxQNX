#ifndef TESTAPP_H_
#define TESTAPP_H_

#include "ofMain.h"
#include "ofxQNX.h"
#include "ofxAccelerometer.h"
#include "Ball.h"

#define NUM_POINTS				10

class testApp : public ofxQNXApp {
	
	public:
		void setup();
		void update();
		void draw();
		void exit();
	
		void touchDown(ofTouchEventArgs &touch);
		void touchMoved(ofTouchEventArgs &touch);
		void touchUp(ofTouchEventArgs &touch);
		void touchDoubleTap(ofTouchEventArgs &touch);
		void touchCancelled(ofTouchEventArgs &touch);


		Ball balls[NUM_POINTS];
};

#endif /* TESTAPP_H_ */
