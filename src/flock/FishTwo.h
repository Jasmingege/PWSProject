#pragma once
#include "ofMain.h"
#include "Boid.h"

class FishTwo :	public Boid
{
public:
	// Constructor
	FishTwo(void);
	// Constructor with given parameters for position
	FishTwo(int x, int y);
	// Destructor
	~FishTwo(void);

	ofImage image;

	void draw();

protected:
private:
};

