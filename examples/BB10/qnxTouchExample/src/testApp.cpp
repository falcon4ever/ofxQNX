#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();

	// initialize all of the Ball particles
	for(int i=0; i<NUM_POINTS; i++) balls[i].init();
}


//--------------------------------------------------------------
void testApp::update() {
	for(int i=0; i<NUM_POINTS; i++) balls[i].update();
}

//--------------------------------------------------------------
void testApp::draw() {
	for(int i=0; i<NUM_POINTS; i++) balls[i].draw();
}

//--------------------------------------------------------------
void testApp::exit() {
	ofLog(OF_LOG_NOTICE,"exit()\n");
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch) {

	int x = touch.x;
	int y = touch.y;
	int id = touch.id;

	fprintf(stderr, "touch %i down at (%i,%i)\n", id, x,y);
	balls[id].moveTo(x, y);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch) {

	int x = touch.x;
	int y = touch.y;
	int id = touch.id;

	fprintf(stderr, "touch %i moved at (%i,%i)\n", id, x, y);
	balls[id].moveTo(x, y);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch) {

	int x = touch.x;
	int y = touch.y;
	int id = touch.id;

	fprintf(stderr, "touch %i up at (%i,%i)\n", id, x, y);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	// Not implemented in listener
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs &touch){
	// Not implemented in listener
}
