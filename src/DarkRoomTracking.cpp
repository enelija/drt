#include "DarkRoomTracking.h"

//--------------------------------------------------------------
void DarkRoomTracking::setup() {

	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
	kinect.init();
	kinect.open();
	
	// print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
	
	// parameters
	parameters.setName("Parameters");
	kinect_tilt.set("Kinect Tilt",0,-30,30);
	nearLimit.set("nearLimit",50,0,255);
	farLimit.set("farLimit",150,0,255);
	threshold.set("Threshold",10,0,255);
	useBackground.set("Background Substraction",false);
	minBlobArea.set("Minimum Blob Area",30,0,100);
	maxBlobArea.set("Maximum Blob Area",90,0,100);

	parameters.add(kinect_tilt);
	parameters.add(nearLimit);
	parameters.add(farLimit);
    parameters.add(threshold);
	parameters.add(minBlobArea);
	parameters.add(maxBlobArea);
    parameters.add(useBackground);

	//gui
	gui.setup(parameters);
	gui.add(grabBackground.setup("grab Background"));
    gui.loadFromFile("settings.xml");
    gui.setPosition(420,360);
	
    //inicialize the images
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	backgroundImage.allocate(kinect.width, kinect.height);
	diffImage.allocate(kinect.width, kinect.height);
	
    //inicialize frame buffer object
    fbo.allocate(kinect.width, kinect.height,GL_RGBA);

    //ofPixels, a data structure to access to the pixel data of an image
    blobPixels.allocate(kinect.width, kinect.height,OF_PIXELS_RGBA);
		
	ofSetFrameRate(60);
		
	sender = new ofxOscSender();
	sender->setup(HOST, PORT);
}

//--------------------------------------------------------------
void DarkRoomTracking::update() {

	ofBackground(100, 100, 100);
	
	kinect.update();
	
    kinect.setCameraTiltAngle(kinect_tilt);

	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		
		if(grabBackground){
            ofLogNotice()<<"learn background";
            backgroundImage = grayImage;
		}
		
        //we have two methods to work, background substraction and showing just the pixels of the image which
        // distance / brightness is between certain values
        if(useBackground){
            diffImage.absDiff(grayImage,backgroundImage);
        }else{
            //put white when the value is in hte range between farlimit and nearLimit
            cvInRangeS(grayImage.getCvImage(),cvScalar(255-farLimit),cvScalar(255-nearLimit),diffImage.getCvImage());
        }
		
        //apply threshold
        diffImage.threshold(threshold);

		// update the cv images
		grayImage.flagImageChanged();
		
        float videoArea = (kinect.width*kinect.height);
		contourFinder.findContours (diffImage, videoArea*minBlobArea/100.0, videoArea*maxBlobArea/100.0, 10, true);
		
		//draw the blobs into the fbo, nd ofFbo in OF is similar to PGraphics in processing
		//everything drawn between fbo.begin() and fbo.end() will be drawn in the fbo object
		//which is a kind a image
        ofPushStyle();
			fbo.begin();
			ofClear(0,0,0,255);

			//we go through all the blobs
			for(int i=0;i<contourFinder.blobs.size();i++){
				ofxCvBlob &blob = contourFinder.blobs[i];
				//draw the blob in a fbo
				ofFill();
				if(blob.hole){
					ofSetHexColor(0x000000);
				}else{
					ofSetHexColor(0xFFFFFF);
				}
				ofBeginShape();
					for (int i = 0; i < blob.nPts; i++){
							ofVertex(blob.pts[i].x,blob.pts[i].y);
					}
				ofEndShape(true);
			}
			fbo.end();
        ofPopStyle();

        fbo.readToPixels(blobPixels); //access to fbo's pixels
        ofColor color;
        userPosition.set(0,0,0);
        int     pixelCounter=0;

        for(int i=0;i<kinect.width;i++){
            for(int j=0;j<kinect.height;j++){
                color = blobPixels.getColor(i,j);
                //if the color is not black we are in a blob, we will add it's position in realworld
                if(color!=0x000000){
                    userPosition+=kinect.getWorldCoordinateAt(i,j);
                    pixelCounter++;
                }
            }
        }
        //the user position is the mean of all the position that forms the blob
        //I think this method is better that just getting the distance of the centroid
        userPosition /= pixelCounter;
		
		// collect last n (AVG_SIZE) values in a deque to calculate the average in 
		// order to make the movement more calm		
		updateAvg(userPositions, userPosition);

		// send position via OSC
		sendPosition(userPosition.x, userPosition.z);
	}
}

//--------------------------------------------------------------
void DarkRoomTracking::updateAvg(std::deque<ofPoint>& positions, ofPoint& position) {
	userPositions.push_back(userPosition);
	if (userPositions.size() > AVG_SIZE)
		userPositions.pop_front();

	position = ofPoint(0.0, 0.0);
	for (std::deque<ofPoint>::iterator it = positions.begin(); it != positions.end(); ++it) {
		position.x += (*it).x;
		position.z += (*it).z;
	}
	position.x /= (float) userPositions.size();
	position.z /= (float) userPositions.size();
}

//--------------------------------------------------------------
void DarkRoomTracking::draw(){

	ofSetColor(255, 255, 255);

	// draw from the live kinect
    kinect.drawDepth(10, 10, 400, 300);
    backgroundImage.draw(420, 10, 200, 150);
		
    diffImage.draw(420, 160, 200, 150);
    contourFinder.draw(420, 160, 200, 150);
    fbo.draw(630,10,200,150);
	        
    //Draw diagram
    ofNoFill();
    ofRect(10,320,350,350);
    ofLine(10+350/2.0,320+350,10+350/2.0,320+350-50);
    if(!(userPosition.x==0 && userPosition.y==0)){
        ofCircle(ofMap(userPosition.x,-1500,1500,10,10+350),ofMap(userPosition.z,0,3000,320+350,320),0,20);
    }
    gui.draw();
} // end of draw()

//--------------------------------------------------------------
void DarkRoomTracking::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void DarkRoomTracking::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
		// the projected points are 'upside down' and 'backwards' 
		ofScale(1, -1, -1);
		ofTranslate(0, 0, -1000); // center the points a bit
		ofEnableDepthTest();
		mesh.drawVertices();
		ofDisableDepthTest();
	ofPopMatrix();
}

//--------------------------------------------------------------
void DarkRoomTracking::sendPosition(float x, float y) {
		ofxOscMessage m;
		m.setAddress("/tracking");
		m.addFloatArg(x);
		m.addFloatArg(y);
		sender->sendMessage(m);
}

//--------------------------------------------------------------
void DarkRoomTracking::keyPressed (int key) {
	switch (key) {
	}
}

//--------------------------------------------------------------
void DarkRoomTracking::keyReleased(int key){

}

//--------------------------------------------------------------
void DarkRoomTracking::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void DarkRoomTracking::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void DarkRoomTracking::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void DarkRoomTracking::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void DarkRoomTracking::windowResized(int w, int h){

}

//--------------------------------------------------------------
void DarkRoomTracking::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void DarkRoomTracking::dragEvent(ofDragInfo dragInfo){ 

}
