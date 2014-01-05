#include "triangleFinder.h"

triangle::triangle(led *_ledA,led *_ledB,led *_ledC ){
  ledA = _ledA;
  ledB = _ledB;
  ledC = _ledC;
}

void triangle::orderLeds(){
	//find the shortest
	float distAB,distBC,distCA;
    distAB = (ledA->position-ledB->position).length();
    distBC = (ledB->position-ledC->position).length();
    distCA = (ledC->position-ledA->position).length();


}

ofPoint triangle::getPosition(){
    
   return ledA->position.getMiddle(ledB->position);
}

float triangle::getOrientation(){
    
	ofPoint rightSide = ledB->position;
	ofPoint leftSide = ledA->position;

	// Shift result by PI to be in range [0, 2*PI] as requested here:
	// http://www.interface.ufg.ac.at/vrprojectwiki/index.php/Communication_Protocol#DARK_ROOM_.E2.86.92_CAVE
	return atan2(leftSide.y-rightSide.y, leftSide.x-rightSide.x) + PI;
}

void triangle::draw(){

    ofPushStyle();
    //triangle contour
    ofBeginShape();
    ofVertex(ledA->position);
    ofVertex(ledB->position);
    ofVertex(ledC->position);
    ofEndShape(OF_CLOSE);

    //orientation vector
    ofPoint position  = this->getPosition();
    float orientationAngle = this->getOrientation();
    ofVec2f orientationVector;
    orientationVector.x = cos(orientationAngle)*30;
    orientationVector.y = sin(orientationAngle)*30;

    ofBeginShape();
    ofVertex(position);
    ofVertex((position + orientationVector).x,(position + orientationVector).y);
    ofEndShape();

    ofPopStyle();
}

triangleFinder::triangleFinder()
{
    //ctor
    triangleCounter = 0;
}

void triangleFinder::findTriangles()
{
    //get a copy of the set of leds
    set<led*> leds = lt->leds;
    set<led*>::iterator led_iterator;
    set<triangle*> foundTriangles;
    set<triangle*>::iterator triangle_iterator;


    for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++){
        //check if triangle is still valid
        if(leds.find((*triangle_iterator)->ledA)!=leds.end() &&
           leds.find((*triangle_iterator)->ledB)!=leds.end() &&
           leds.find((*triangle_iterator)->ledC)!=leds.end()){
                leds.erase((*triangle_iterator)->ledA);
                leds.erase((*triangle_iterator)->ledB);
                leds.erase((*triangle_iterator)->ledC);
                foundTriangles.insert(*triangle_iterator);
        }else{
            //if the triangle is not  valid we return its memory
            delete *triangle_iterator;
        }
    }
    //at this point in leds we haves leds that don't belong to any triangle
    while(leds.size()>=3){
        led *ledA,*ledB,*ledC;
        float mindistSQ = FLT_MAX;
        ledA = *leds.begin();
        ledB=ledC=0;
        leds.erase(ledA);
        for (led_iterator = leds.begin(); led_iterator != leds.end(); led_iterator++){
            ofPoint distVector = (*led_iterator)->position - ledA->position;
            float distSQ = distVector.x*distVector.x+distVector.y*distVector.y;
            if(distSQ<mindistSQ){
                //we want to keep the nearest point to ledA in ledB and the next nearest point in ledC
                ledC=ledB;
                ledB=*led_iterator;
            }
        }
        triangle *t = new triangle(ledA,ledB,ledC);
        t->id = triangleCounter;
        triangleCounter++;
        foundTriangles.insert(t);
    }
    triangles = foundTriangles;
}

void triangleFinder::setLedTracker(ledTracker &_lt)
{
    lt = &_lt;
}

void triangleFinder::draw( float x, float y, float w, float h ) {

    float scalex = 0.0f;
    float scaley = 0.0f;

    float _width  = lt->width;
    float _height = lt->height;
    set<triangle*>::iterator triangle_iterator;

    if( _width != 0 ) { scalex = w/_width; } else { scalex = 1.0f; }
    if( _height != 0 ) { scaley = h/_height; } else { scaley = 1.0f; }

    ofPushStyle();
    // ---------------------------- draw the bounding rectangle
    ofSetHexColor(0xDD00CC);
    glPushMatrix();
    glTranslatef( x, y, 0.0 );
    glScalef( scalex, scaley, 0.0 );

    ofNoFill();

    // ---------------------------- draw the blobs
    ofSetHexColor(0x00FFFF);

    for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++){
        stringstream str;
        str <<"triangle:"<<(*triangle_iterator)->id;
        ofPoint position = (*triangle_iterator)->getPosition();
        ofDrawBitmapString(str.str(),position.x,position.y);
        (*triangle_iterator)->draw();
    }
    glPopMatrix();
    ofPopStyle();
}
