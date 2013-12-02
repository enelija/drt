#include "DarkRoomTracking.h"

//--------------------------------------------------------------
void DarkRoomTracking::setup(){
	
	camWidth		= 320;			// 1024;			
	camHeight		= 240;			// 768;
	frameRate		= 30;
	#ifdef _USE_LIVE_VIDEO
	threshold		= 240;
	minArea			= 20; 
	#else
	threshold		= 80;
	minArea			= 1; 
	#endif
	maxArea			= camWidth * camHeight / 16;
	nConsidered		= 10; 
    filepaths.push_back("single.mp4");
    filepaths.push_back("single_flicker.mp4");
    filepaths.push_back("twins.mp4");
    filepaths.push_back("twins_flicker.mp4");

	sender = new ofxOscSender();
	sender->setup(HOST, PORT);

    filepath_iterator = filepaths.begin();
	#ifdef _USE_LIVE_VIDEO
		vector<ofVideoDevice> devices = vidGrabber.listDevices();
		for(int i = 0; i < devices.size(); ++i) {
			cout << devices[i].id << ": " << devices[i].deviceName; 
			if (devices[i].bAvailable) {
				cout << endl;
			} else {
				cout << " - unavailable " << endl; 
			}
		}
    
        vidGrabber.setVerbose(true);
		vidGrabber.setDeviceID(0);
		vidGrabber.setDesiredFrameRate(frameRate);
		vidGrabber.initGrabber(camWidth,camHeight);
	#else
        vidPlayer.loadMovie(*filepath_iterator);
        vidPlayer.setLoopState(OF_LOOP_NORMAL);
        vidPlayer.play();
	#endif

    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);

    lt.setContourFinder(contourFinder);
    tf.setLedTracker(lt);
	bLearnBakground = false;
}

//--------------------------------------------------------------
void DarkRoomTracking::update(){
	ofBackground(100,100,100);

    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.update();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), 320, 240);
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels(), 320, 240);
        #endif

        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		//grayDiff.absDiff(grayBg, grayImage);				// working better without for LEDs
		//grayDiff = grayImage;
		//grayDiff.threshold(threshold);
		grayImage.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/16 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		//contourFinder.findContours(grayDiff, minArea, maxArea, nConsidered, false);	// find holes
		contourFinder.findContours(grayImage, minArea, maxArea, nConsidered, false);	// find holes
		lt.trackLeds();
		tf.findTriangles();
		
		set<triangle*>::iterator it = tf.triangles.begin();
		if (tf.triangles.size() == 1)
			sendPositionAndOrientation((*it)->getPosition().x, (*it)->getPosition().x, (*it)->getOrientation());
		// TODO: choose the "best" triangle fit since we only want a single triangle
	}
}

//--------------------------------------------------------------
void DarkRoomTracking::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	colorImg.draw(20,20);

	// then draw the contours:
    int contourX = 360;
    int contourY = 20;
	
	ofFill();
	ofSetHexColor(0x333333);
	ofRect(contourX,contourY,320,240);
	ofSetHexColor(0xffffff);

	// or, instead we can draw each blob individually from the blobs vector,
	// this is how to get access to them:
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(contourX,contourY);

		// draw over the centroid if the blob is a hole
		ofSetColor(255);
		if(contourFinder.blobs[i].hole){
			ofDrawBitmapString("hole",
				contourFinder.blobs[i].boundingRect.getCenter().x + contourX,
				contourFinder.blobs[i].boundingRect.getCenter().y + contourY);
		}
    }

    // we  draw the led tracker and the triangle finder in the same place
	lt.draw(700,20,320,240);
	tf.draw(700,20,320,240);

	// finally, a report:
	ofSetHexColor(0xffffff);
	stringstream reportStr;
	reportStr << "press ' ' to switch video" << endl
			  << "threshold " << threshold << " (press: +/-)" << endl
			  << "video: "<< *filepath_iterator<<" fps: " << ofGetFrameRate()<<endl;

	ofDrawBitmapString(reportStr.str(), 20, 350);

    stringstream trianglesStr;
    trianglesStr.precision(2);
    trianglesStr << "num triangles found " << tf.triangles.size() << endl;
    for (set<triangle*>::iterator triangle_iterator = tf.triangles.begin(); triangle_iterator != tf.triangles.end(); triangle_iterator++){
         trianglesStr   <<"triangle id:"<<(*triangle_iterator)->id
                        <<" orientation:"<<fixed<<(*triangle_iterator)->getOrientation()
                        <<" (rad) / "<<fixed<<(*triangle_iterator)->getOrientation()*180.0f/PI
                        <<" (degree) position: x:"<<fixed<<(*triangle_iterator)->getPosition().x
                        <<", y:"<<fixed<<(*triangle_iterator)->getPosition().y<<endl;

    }
    ofDrawBitmapString(trianglesStr.str(), 400, 350);
}

//--------------------------------------------------------------

void DarkRoomTracking::keyPressed(int key){

	switch (key){
#ifndef _USE_LIVE_VIDEO
		case ' ':
            filepath_iterator++;
            if(filepath_iterator==filepaths.end()){
                filepath_iterator=filepaths.begin();
            }
            vidPlayer.stop();
            vidPlayer.loadMovie(*filepath_iterator);
            vidPlayer.play();
			break;
#endif
		case '+': 
		case OF_KEY_UP:
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-': 
		case OF_KEY_DOWN:
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

void DarkRoomTracking::sendPositionAndOrientation(float x, float y, float orientation) {
		ofxOscMessage m;
		m.setAddress("/tracking");
		m.addFloatArg(x);
		m.addFloatArg(y);
		m.addFloatArg(orientation);
		sender->sendMessage(m);
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
