#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxOsc.h"
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
		
		void drawPointCloud();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void sendPosition(float x, float y);

		ofxKinect kinect;
	
		ofxCvColorImage colorImg;
		ofxCvGrayscaleImage grayImage; // grayscale depth image
		ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
		ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
		ofxCvContourFinder contourFinder;

		ofPoint position;
		ofVec3f worldPosition;

		float kinect_px;
		float kinect_py;
	
		bool bThreshWithOpenCV;
		bool bDrawPointCloud;
		int nearThreshold;
		int farThreshold;
		int angle;
	
		ofEasyCam easyCam;

		ofxOscSender* sender;
};
