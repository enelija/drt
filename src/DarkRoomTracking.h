#pragma once

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxOsc.h"
#include "ofxGui.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

#define HOST "localhost"
#define PORT 12347

class DarkRoomTracking : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void drawPointCloud();
		void sendPosition(float x, float y);

		ofxKinect kinect;
	
		ofxCvColorImage colorImg;
		
		ofxCvGrayscaleImage grayImage;			// grayscale depth image
		ofxCvGrayscaleImage backgroundImage;	// bg image, for substraction
		ofxCvGrayscaleImage diffImage;			// difference image 

		ofxCvContourFinder contourFinder;
		
		ofFbo fbo;
		
		ofPoint userPosition;
							
		ofxPanel gui;
		ofParameterGroup parameters;

		ofParameter<float>  kinect_tilt;
		ofParameter<int>    nearLimit;
		ofParameter<int>    farLimit;
		ofParameter<int>    threshold;
		ofParameter<int>    minBlobArea;
		ofParameter<int>    maxBlobArea;
		ofParameter<bool>   useBackground;
		
		ofxButton grabBackground;

		ofPixels blobPixels;
			
		ofxOscSender* sender;
};
