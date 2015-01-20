#include "FishOne.h"

FishOne::FishOne(void)
{
    image.loadImage("fishone.png");         // load the image for Fishone

	setPosition(ofRandomWidth(),ofRandomHeight());  // sets starting random location
    acc.set(0,0);
    r = 3.0;
    maxspeed = 4;
    maxforce = 0.2;
    bgcolor = 3;
	life = 10;
}

FishOne::FishOne(int x, int y)
{
	image.loadImage("fishone.png");         // load the image for Fishone

	setPosition(x, y);  // sets starting random location
    acc.set(0,0);
    r = 3.0;
    maxspeed = 4;
    maxforce = 0.2;
    bgcolor = 3;
}


FishOne::~FishOne(void)
{
}



void FishOne::draw() 
{
	float angle = (float)atan2(-getVelocity().y, getVelocity().x);
    float theta =  -1.0*angle;
    float heading2D = ofRadToDeg(theta)+90;

    ofPushStyle();
		ofFill();
		ofPushMatrix();
			ofTranslate(getPosition().x, getPosition().y);
			ofRotateZ(heading2D);
			image.draw(0,0);       // draws the image on Fishone
		ofPopMatrix();
    ofPopStyle();
}
