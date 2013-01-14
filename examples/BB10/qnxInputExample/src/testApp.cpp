#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	ofSetCircleResolution(50);
	ofBackground(255,255,255);

	x1 = 200;
	y1 = 200;

	x2 = 200;
	y2 = 400;

	x3 = 824;
	y3 = 200;

	x4 = 824;
	y4 = 400;

	radius = 75;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	ofFill();		// draw "filled shapes"

	ofSetColor(255, 0, 0);
	ofCircle(x1, y1, radius);

	ofSetColor(0, 0, 255);
	ofCircle(x2, y2, radius);

	ofSetColor(0, 255, 0);
	ofCircle(x3, y3, radius);

	ofSetColor(255, 0, 255);
	ofCircle(x4, y4, radius);

	ofSetLineWidth(4);

	ofSetColor(0, 255, 255);
	ofLine(x1, y1, x2, y2);
	ofLine(x1, y1, x3, y3);
	ofLine(x1, y1, x4, y4);
	ofLine(x2, y2, x3, y3);
	ofLine(x2, y2, x4, y4);
	ofLine(x3, y3, x4, y4);
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch) {

	if(touch.id == 0)
	{
		x1 = touch.x;
		y1 = touch.y;
	}
	else if(touch.id == 1)
	{
		x2 = touch.x;
		y2 = touch.y;
	}
	else if(touch.id == 2)
	{
		x3 = touch.x;
		y3 = touch.y;
	}
	else if(touch.id == 3)
	{
		x4 = touch.x;
		y4 = touch.y;
	}
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch) {

	if(touch.id == 0)
	{
		x1 = touch.x;
		y1 = touch.y;
	}
	else if(touch.id == 1)
	{
		x2 = touch.x;
		y2 = touch.y;
	}
	else if(touch.id == 2)
	{
		x3 = touch.x;
		y3 = touch.y;
	}
	else if(touch.id == 3)
	{
		x4 = touch.x;
		y4 = touch.y;
	}
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch) {

	if(touch.id == 0)
	{
		x1 = touch.x;
		y1 = touch.y;
	}
	else if(touch.id == 1)
	{
		x2 = touch.x;
		y2 = touch.y;
	}
	else if(touch.id == 2)
	{
		x3 = touch.x;
		y3 = touch.y;
	}
	else if(touch.id == 3)
	{
		x4 = touch.x;
		y4 = touch.y;
	}
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	// Not implemented in listener
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs &touch){
	// Not implemented in listener
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
