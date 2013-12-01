#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"

#include "ledTracker.h"
#include "triangleFinder.h"

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
							// otherwise, we'll use a movie file

#define HOST "localhost"
#define PORT 12347

class DarkRoomTracking : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void setupOld();
		void updateOld();
		void drawOld();
		void keyPressedOld(int key);
		void sendPositionAndOrientation(float x, float y, float orientation);
		
        #ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
		#else
		  ofVideoPlayer 		vidPlayer;
		#endif
		  
        vector<string>           filepaths;
        vector<string>::iterator filepath_iterator;

		int 					camWidth;
		int 					camHeight;
		int						frameRate;
		float					threshold;
		bool					bLearnBakground;
		
		ofxCvColorImage			colorImg;
		ofxCvGrayscaleImage		grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

		ofxCvContourFinder		contourFinder;

		std::vector<ofRectangle> bRect;
		// Smallest potential blob size as measured in pixels
		int						minArea; 
		// Largest potential blob size as measured in pixels
		int						maxArea; 
		// Maximum numbers of blobs to consider
		int						nConsidered; 
				
        ledTracker          lt;
        triangleFinder      tf;
		
		ofxOscSender* sender;
};
