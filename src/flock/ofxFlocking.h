
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
    void addBoid(int x, int y);

	vector<ofPtr<Boid>> boids;
};
