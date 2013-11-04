#include "DarkRoomTracking.h"

//--------------------------------------------------------------
void DarkRoomTracking::setup() {

	camWidth		= 512;			// 1024;			
	camHeight		= 384;			// 768;
	frameRate		= 30;
	colorChannels	= 1;			// The DMK31AF03 camera is a monochrome camera
	
    // we can now get back a list of devices. 
	vector<ofVideoDevice> devices = vidGrabber.listDevices();
	
    for(int i = 0; i < devices.size(); ++i) {
		cout << devices[i].id << ": " << devices[i].deviceName; 
        if (devices[i].bAvailable) {
            cout << endl;
        } else {
            cout << " - unavailable " << endl; 
        }
	}
    
	vidGrabber.setDeviceID(0);
	vidGrabber.setDesiredFrameRate(frameRate);
	vidGrabber.initGrabber(camWidth,camHeight);
	
	videoProcessed 	= new unsigned char[camWidth * camHeight * colorChannels];
	videoTexture.allocate(camWidth, camHeight, GL_RGB);	
	
	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void DarkRoomTracking::update() {
	
	ofBackground(100,100,100);
	
	vidGrabber.update();
	
	if (vidGrabber.isFrameNew()) {
		int totalPixels = camWidth * camHeight;
		unsigned char * pixels = vidGrabber.getPixels();
		for (int i = 0; i < totalPixels; ++i) {
			videoProcessed[i] = pixels[i * colorChannels];
		}
		videoTexture.loadData(videoProcessed, camWidth, camHeight, GL_LUMINANCE);
	}
}

//--------------------------------------------------------------
void DarkRoomTracking::draw() {
	
	ofSetHexColor(0xffffff);
	vidGrabber.draw(0, 0);
	videoTexture.draw(camWidth, 0, camWidth,camHeight);
}

//--------------------------------------------------------------
void DarkRoomTracking::keyPressed(int key) {

	// in fullscreen mode, on a pc at least, the 
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...
	
	// Video settings no longer works in 10.7
	// You'll need to compile with the 10.6 SDK for this
    // For Xcode 4.4 and greater, see this forum post on instructions on installing the SDK
    // http://forum.openframeworks.cc/index.php?topic=10343        
	if (key == 's' || key == 'S') {
		vidGrabber.videoSettings();
	}
}

//--------------------------------------------------------------
void DarkRoomTracking::keyReleased(int key) {

}

//--------------------------------------------------------------
void DarkRoomTracking::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void DarkRoomTracking::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void DarkRoomTracking::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void DarkRoomTracking::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void DarkRoomTracking::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void DarkRoomTracking::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void DarkRoomTracking::dragEvent(ofDragInfo dragInfo) { 

}