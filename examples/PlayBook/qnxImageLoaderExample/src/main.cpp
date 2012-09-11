#include "ofMain.h"
#include "testApp.h"

// Windows
// #include "ofAppGlutWindow.h"

//========================================================================
int main() {

	// Windows
    // ofAppGlutWindow window;
	// ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context

	ofAppQNXWindow window;
	ofSetupOpenGL(&window, 1024, 600, OF_WINDOW);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

	return 0;
}
