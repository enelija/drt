#ifndef LEDTRACKER_H
#define LEDTRACKER_H

#include "ofMain.h"
#include "ofxOpenCv.h"
/*
led, a posititon, an id and a counter
*/

class led
{
    public:
        led(int _id);

        ofPoint position;
        int lostFrames; //number of frames since the last time the led was seen
        int id;
};
/*
 with the blobs of a contourfinder tries to keep track of the all the leds and stores them in a set
*/
class ledTracker
{
    public:
        ledTracker();
        void setContourFinder(ofxCvContourFinder &_ct);
        void draw( float x, float y, float w, float h );
        set<led*> leds;
        void trackLeds();
        float maxDistance; //max distance between a led in two different frames to be considered the same led
        float width,height;
    protected:
        ofxCvContourFinder *contourFinder;
        int ledCounter;
    private:
};

#endif // LEDTRACKER_H
