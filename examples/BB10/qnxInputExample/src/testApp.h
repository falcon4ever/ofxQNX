#ifndef TESTAPP_H_
#define TESTAPP_H_

#include "ofMain.h"
#include "ofxQNX.h"

class testApp : public ofxQNXApp {
	
	public:
		void setup();
		void update();
		void draw();
		
		void touchDown(ofTouchEventArgs &touch);
		void touchMoved(ofTouchEventArgs &touch);
		void touchUp(ofTouchEventArgs &touch);
		void touchDoubleTap(ofTouchEventArgs &touch);
		void touchCancelled(ofTouchEventArgs &touch);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

	private:
		float radius;

		int x1, y1;
		int x2, y2;
		int x3, y3;
		int x4, y4;
};
	
#endif /* TESTAPP_H_ */
