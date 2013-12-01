#ifndef TRIANGLEFINDER_H
#define TRIANGLEFINDER_H

#include "ledTracker.h"

//a basic structure storing a triangle formed by 3 leds
//it has a function to return its position and its orientation

class triangle{
    public:

        int id;
        led *ledA,*ledB,*ledC;

        triangle(led *_ledA,led *_ledB,led *_ledC );
        float getOrientation();
        ofPoint getPosition();
        void draw();
};
//from a set of leds tries to find a keep track of the triangles found.

class triangleFinder
{
    public:
        triangleFinder();
        void findTriangles();
        void setLedTracker(ledTracker &_lt);
        void draw( float x, float y, float w, float h );
        int triangleCounter;
        ledTracker *lt;
        set<triangle*> triangles;
};

#endif // TRIANGLEFINDER_H
