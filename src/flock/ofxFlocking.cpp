
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

