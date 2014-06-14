#pragma once

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxOsc.h"
#include "ofxGui.h"

#include <deque>

#define HOST "localhost"
#define PORT 12347
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
//number of values to calculate the avegare position
#define AVG_SIZE 10
//scale for maxBlob minBlob parameters each unit is 1/BLOB_SCALE
#define BLOB_SCALE 1000.0
//number of frames without detecting the user to consider the user is gone
#define MAX_USER_FRAMES 10

class DarkRoomTracking : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		void exit();
		
		void drawPointCloud();
		void sendPosition(float x, float y);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		

		ofxKinect kinect;
	
		ofxCvColorImage colorImg;
		
		ofxCvGrayscaleImage grayImage;			// grayscale depth image
		ofxCvGrayscaleImage backgroundImage;	// bg image, for substraction
		ofxCvGrayscaleImage diffImage;			// difference image 

		ofxCvContourFinder contourFinder;
		
		ofFbo fbo;
		float scale;
		
		int framesSinceUserDetected; //num of frames since the user was detected last time
		ofPoint userPosition;
		std::deque<ofPoint> userPositions;
		void updateAvg(std::deque<ofPoint>& positions, ofPoint& position);
							
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
