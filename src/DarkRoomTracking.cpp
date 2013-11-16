#include "DarkRoomTracking.h"

//--------------------------------------------------------------
void DarkRoomTracking::setup() {

	// the DMK31AF03 camera is a monochrome camera, so 1 channel, color cameras use 3 channels
	colorChannels	= 3;			
	camWidth		= 512;			// 1024;			
	camHeight		= 384;			// 768;
	frameRate		= 30;
	threshold		= 248;			// threshold for binarization
	numOfLeds		= 3;
	updateTestStep	= 0.1f;
	minArea			= 0; 
	maxArea			= (camWidth*camHeight)/4;
	nConsidered		= 10; 
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
	
	//videoMonochrome = new unsigned char[camWidth * camHeight];
	//videoBinarized = new unsigned char[camWidth * camHeight];
		
	//videoTextureMonochrome.allocate(camWidth, camHeight, GL_LUMINANCE);
	//videoTextureBinarized.allocate(camWidth, camHeight, GL_LUMINANCE);

	colorImg.allocate(camWidth,camHeight);
	grayImage.allocate(camWidth,camHeight);
	gbImage.allocate(camWidth,camHeight);

	ofSetVerticalSync(true);
	generateTestLedCentroids();
}

//--------------------------------------------------------------
void DarkRoomTracking::update() {
	
	ofBackground(100,100,100);
	
	vidGrabber.update();	
	ofxCvBlob				blobs;


	//int totalPixels = camWidth * camHeight;
	//unsigned char * pixels = vidGrabber.getPixels(); 


	if (vidGrabber.isFrameNew()) 
	{
		colorImg.setFromPixels(vidGrabber.getPixelsRef());
		//colorImg.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);
		grayImage = colorImg;
		gbImage = grayImage;

		/*for (int i = 0; i < totalPixels; ++i) {
			// for color cameras use the first color channel for now
			videoMonochrome[i] = pixels[i * colorChannels];
			if (pixels[i * colorChannels] > threshold)
				videoBinarized[i] = 255;
			else
				videoBinarized[i] = 0;
		}
		videoTextureMonochrome.loadData(videoMonochrome, camWidth, camHeight, GL_LUMINANCE);
		videoTextureBinarized.loadData(videoBinarized, camWidth, camHeight, GL_LUMINANCE);*/
	
		gbImage.threshold(threshold);

		contourfinder.findContours(gbImage, minArea, maxArea, nConsidered, false, true);
		ledCentroids.empty();
		bRect.empty();

		for(int i = 0; i < contourfinder.nBlobs; i++)
		{
			blobs = contourfinder.blobs.at(i);
			ledCentroids.push_back(blobs.centroid);
			bRect.push_back(blobs.boundingRect);
		}

	}
}

//--------------------------------------------------------------
/*void DarkRoomTracking::draw() {
	
	ofSetHexColor(0xffffff);
	vidGrabber.draw(0, 0);
	videoTextureMonochrome.draw(camWidth, 0, camWidth,camHeight);
	videoTextureBinarized.draw(0, camHeight, camWidth,camHeight);
	ofSetColor(255, 0, 0);
	font.drawString("threshold: " + ofToString(threshold), 10, 2 * camHeight - 10);
}*/

void DarkRoomTracking::draw(){

	// draw the incoming, the grayscale
	ofSetHexColor(0xffffff);
	vidGrabber.draw(0, 0);
	grayImage.draw(camWidth,0,camWidth,camHeight);
	gbImage.draw(0,camHeight,camWidth,camHeight);
	

	// we could draw the whole contour finder
	//contourfinder.draw(360,540);

	// or, instead we can draw each blob individually from the blobs vector,
	// this is how to get access to them:
    for (int i = 0; i < contourfinder.nBlobs; i++){
		contourfinder.blobs[i].draw(camWidth,camHeight);
		
    }

	// finally, a report:
	ofSetColor(255,0,0);
	//font.drawString("threshold: " + ofToString(threshold) +"\n", 10, 2 * camHeight - 10);
	font.drawString("blobs: " + ofToString(contourfinder.nBlobs), 10, 2 * camHeight - 10);

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
	float rand = ofRandom(-updateTestStep, updateTestStep);
	for (int i = ledCentroids.size() - 1; i > 0; --i)
		ledCentroids[i] += rand;
}