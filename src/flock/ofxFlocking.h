
#pragma once

#include "ofMain.h"
#include "Boid.h"

class ofxFlocking
{
public:
    //methods
    void update();
    void draw();
    void addBoid();

	vector<ofPtr<Boid>> boids;
};
