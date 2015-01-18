
#include "ofxFlocking.h"
#include "FishOne.h"
#include "FishTwo.h"
#include "..\ofApp.h"

//function calls the update function of the position of the boids
void ofxFlocking::update()
{
    for(int i=0; i<boids.size(); i++)
    {
        boids[i]->update(boids);
    }
}

//function calls the draw function of the boids
void ofxFlocking::draw()
{
    for(int i=0; i<boids.size(); i++)
    {
        boids[i]->draw();
    }
}

//function which creates a new boid
void ofxFlocking::addBoid()
{
    boids.push_back(ofPtr<FishOne>(new FishOne));
    boids.push_back(ofPtr<FishTwo>(new FishTwo));
}

//function which created a new boid with given parameters for the position
void ofxFlocking::addBoid(int x, int y)
{
    boids.push_back(ofPtr<FishOne>(new FishOne(x, y)));
    boids.push_back(ofPtr<FishTwo>(new FishTwo(x, y)));
}

