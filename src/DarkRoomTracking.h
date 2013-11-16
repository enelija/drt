#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

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
		void generateTestLedCentroids();
		void updateTestLedCentroids();

		ofVideoGrabber 			vidGrabber;
		//unsigned char * 		videoMonochrome;
		//unsigned char * 		videoBinarized;
		ofTexture				videoTextureMonochrome;
		ofTexture				videoTextureBinarized;
		int 					camWidth;
		int 					camHeight;
		int						frameRate;
		int						colorChannels;
		float					threshold;
		ofTrueTypeFont			font;

		std::vector<ofPoint>	ledCentroids;
		int						numOfLeds;
		float					updateTestStep;

		bool					bLearnBakground;
		ofxCvColorImage			colorImg;
		ofxCvGrayscaleImage		grayImage;
		ofxCvGrayscaleImage		gbImage;
		ofxCvContourFinder		contourfinder;
		std::vector<ofRectangle> bRect;
		// Smallest potential blob size as measured in pixels
		int						minArea; 
		// Largest potential blob size as measured in pixel
		int						maxArea; 
		// Maximum numbers of blobs to consider
		int						nConsidered; 



};
