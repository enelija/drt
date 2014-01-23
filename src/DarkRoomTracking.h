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
#define AVG_SIZE 10

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
		
		int avgVals;
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
