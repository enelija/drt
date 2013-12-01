#include "ofAppGlutWindow.h"
#include "ofMain.h"
#include "DarkRoomTracking.h"

int main( ){

	ofSetupOpenGL(1024, 768, OF_WINDOW);
	ofRunApp(new DarkRoomTracking());
}
