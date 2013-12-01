#include "ledTracker.h"

//we could init the values using list
//http://www.learncpp.com/cpp-tutorial/101-constructor-initialization-lists/


led::led(int _id)
{
    id = _id;
    lostFrames=0;
}

ledTracker::ledTracker()
{
    ledCounter = 0;
}

void ledTracker::setContourFinder(ofxCvContourFinder &_ct)
{
    contourFinder = &_ct;
    width   = contourFinder->getWidth();
    height  = contourFinder->getHeight();
}


void ledTracker::trackLeds()
{
    std::set<led*> foundLeds;
    std::set<led*>::iterator led_iterator;

    //iterate through the blobs
    for (size_t i = 0; i < contourFinder->blobs.size(); i++)
    {
        //find its nearest led
        float minDistSQ = FLT_MAX; //here we could define max distanceSQ
        led   *nearestLed = 0; //null pointer

        for (led_iterator = leds.begin(); led_iterator != leds.end(); led_iterator++){
            ofPoint distVector = (*led_iterator)->position - contourFinder->blobs[i].centroid;
            float distSQ = distVector.x*distVector.x+distVector.y*distVector.y;
            if( distSQ < minDistSQ){
                minDistSQ = distSQ;
                nearestLed = *led_iterator;
            }
        }
        //We found a near led for this blob
        if(nearestLed!=0){
            nearestLed->lostFrames=0;
            nearestLed->position = contourFinder->blobs[i].centroid;
            foundLeds.insert(nearestLed);
            leds.erase(nearestLed);
        }else{
            ledCounter++;
            led *l = new led(ledCounter);
            l->position = contourFinder->blobs[i].centroid;
            foundLeds.insert(l);
        }
    }

    //at this point all the leds that are still in the led list have not been found in this frame
    for (led_iterator = leds.begin(); led_iterator != leds.end(); led_iterator++){
        (*led_iterator)->lostFrames++;
        if((*led_iterator)->lostFrames<10){
            foundLeds.insert(*led_iterator);
        }
    }
    leds = foundLeds;
}

void ledTracker::draw( float x, float y, float w, float h ) {

    float scalex = 0.0f;
    float scaley = 0.0f;
    float _width  = contourFinder->getWidth();
    float _height = contourFinder->getHeight();
    std::set<led*>::iterator led_iterator;


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

    for (led_iterator = leds.begin(); led_iterator != leds.end(); led_iterator++){
        stringstream str;
        str <<"led:"<<(*led_iterator)->id;
        ofDrawBitmapString(str.str(),(*led_iterator)->position.x,(*led_iterator)->position.y);
    }
    glPopMatrix();
    ofPopStyle();
}
