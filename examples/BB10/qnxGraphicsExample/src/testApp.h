#ifndef TESTAPP_H_
#define TESTAPP_H_

#include "ofMain.h"
#include "ofxQNX.h"

class testApp : public ofxQNXApp {
	
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
		
		float 	counter;
		bool	bSmooth;
};
	
#endif /* TESTAPP_H_ */
