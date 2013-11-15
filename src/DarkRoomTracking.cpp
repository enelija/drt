#include "DarkRoomTracking.h"

//--------------------------------------------------------------
void DarkRoomTracking::setup() {

	// the DMK31AF03 camera is a monochrome camera, so 1 channel, color cameras use 3 channels
	colorChannels	= 3;			
	camWidth		= 512;			// 1024;			
	camHeight		= 384;			// 768;
	frameRate		= 30;
	threshold		= 128;			// threshold for binarization
	numOfLeds		= 3;
	updateTestStep	= 0.1f;
	
	font.loadFont("arial.ttf", 12);

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
	
	videoMonochrome = new unsigned char[camWidth * camHeight];
	videoBinarized = new unsigned char[camWidth * camHeight];
		
	videoTextureMonochrome.allocate(camWidth, camHeight, GL_LUMINANCE);
	videoTextureBinarized.allocate(camWidth, camHeight, GL_LUMINANCE);

	ofSetVerticalSync(true);

	generateTestLedCentroids();
}

//--------------------------------------------------------------
void DarkRoomTracking::update() {
	
	ofBackground(100,100,100);
	
	vidGrabber.update();
	
	if (vidGrabber.isFrameNew()) {
		int totalPixels = camWidth * camHeight;
		unsigned char * pixels = vidGrabber.getPixels();
		for (int i = 0; i < totalPixels; ++i) {
			// for color cameras use the first color channel for now
			videoMonochrome[i] = pixels[i * colorChannels];
			if (pixels[i * colorChannels] > threshold)
				videoBinarized[i] = 255;
			else
				videoBinarized[i] = 0;
		}
		videoTextureMonochrome.loadData(videoMonochrome, camWidth, camHeight, GL_LUMINANCE);
		videoTextureBinarized.loadData(videoBinarized, camWidth, camHeight, GL_LUMINANCE);
	}
}

//--------------------------------------------------------------
void DarkRoomTracking::draw() {
	
	ofSetHexColor(0xffffff);
	vidGrabber.draw(0, 0);
	videoTextureMonochrome.draw(camWidth, 0, camWidth,camHeight);
	videoTextureBinarized.draw(0, camHeight, camWidth,camHeight);
	ofSetColor(255, 0, 0);
	font.drawString("threshold: " + ofToString(threshold), 10, 2 * camHeight - 10);
}

//--------------------------------------------------------------
void DarkRoomTracking::keyPressed(int key) {

	if (key == 's' || key == 'S') {
		vidGrabber.videoSettings();
	} else if (key == OF_KEY_UP && threshold < 255) {
		threshold += 1;
		cout << "threshold: " << threshold << endl;
	} else if (key == OF_KEY_DOWN && threshold > 0) {
		threshold -= 1;
		cout << "threshold: " << threshold << endl;
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

void DarkRoomTracking::generateTestLedCentroids() {
	for (int i = 0; i < numOfLeds; ++i)
		ledCentroids.push_back(ofPoint(ofRandom(0.0f, 1.0f), ofRandom(0.0f, 1.0f)));
}

void DarkRoomTracking::updateTestLedCentroids() {
	for (int i = ledCentroids.size() - 1; i > 0; --i)
		ledCentroids[i] += ofRandom(-updateTestStep, updateTestStep);
}