#pragma once
#include "boid.h"
class FishOne : public Boid
{
public:
	// constructor
    FishOne();
    // constructor with given parameters for the position
    FishOne(int x, int y);
	// Destructor
	~FishOne(void);
	
	ofImage image;

	void draw();

protected:
private:
};