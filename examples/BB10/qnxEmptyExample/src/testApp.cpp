#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	// Setup multitouch input listener
	ofRegisterTouchEvents(this);

	// Set background color
	ofBackground(255,255,255);

	// Setup variables
}

//--------------------------------------------------------------
void testApp::update(){
	// Update logics and variables here
}

//--------------------------------------------------------------
void testApp::draw() {
	// Draw code only
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	// Touch input
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	// Touch input
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	// Touch input
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	// Not implemented in touch listener
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs &touch){
	// Not implemented in touch listener
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
