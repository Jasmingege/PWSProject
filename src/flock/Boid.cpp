

#include "..\ofApp.h"
#include "boid.h"

// Definition of the superclass Boid constructor
Boid::Boid()
{
    setPosition(ofRandomWidth(),ofRandomHeight());  // sets starting random location
    acc.set(0,0);
    maxspeed = 4;
    maxforce = 0.2;
	age=0;
	isDead= false;
	maxAge=100;
}

//Definition of the superclass Boid constructor with given parameters for the position
Boid::Boid(int x, int y)
{
	/*
	Old non Box2d code
    loc.set(x,y);
    vel.set(0,0);
    */

	setPosition(x, y);		// Set Box2D position
	setVelocity(0, 0);		// Set Box2D velocity

	acc.set(0,0);

    maxspeed = 4;
    maxforce = 0.2;
	age=0;
	isDead=false;
	maxAge=100;
	
}


// Defiition of the superclass Boid method for updating the location
void Boid::update(vector<ofPtr<Boid>> boids)
{
    /*
	Old position code, we now use the position and velocity of the ofxBox2dBaseShape class.

	flock(boids);

    vel += acc;                                     // Update velocity
    vel.x = ofClamp(vel.x, -maxspeed, maxspeed);    // Limit speed
    vel.y = ofClamp(vel.y, -maxspeed, maxspeed);    // Limit speed
    loc += vel;                                     // location of the boids
    acc.set(0,0);                                   // Reset accelertion to 0 each cycle

    if (loc.x < -r) loc.x = ofGetWidth()+r;
    if (loc.y < -r) loc.y = ofGetHeight()+r;

    if (loc.x > ofGetWidth()+r) loc.x = -r;
    if (loc.y > ofGetHeight()+r) loc.y = -r;
	*/
	
	flock(boids);									// Flock with the boyds and update acceleration and velocity

	addForce(acc, 1);								// Set Box2D velocity
	
	// Note: We don't have to set constraints for the flocking since Box2D world does that for us.
}

//definition of the superclass Boid method to seek for another Boid boids
void Boid::seek(ofVec2f target)
{
    acc += steer(target, false);
}

//definition of the superclass Boid method to avoid an enemy
void Boid::avoid(ofVec2f target)
{
    acc -= 2*steer(target, false);
}

// Definition of the superclass Boid method to swim to another Boid boids
void Boid::arrive(ofVec2f target)
{
    acc += steer(target, true);
}

// Definition of the superclass Boid method that calculates a steering vector towards another Boid boids
// Slows down as it approaches the other Boid boids, using the second argument (if true- slows down)
ofVec2f Boid::steer(ofVec2f target, bool slowdown)
{
    ofVec2f steer;															// The steering vector
    ofVec2f desired = target - getPosition();								// Vector pointing from the location to the target

    float d = ofDist(target.x, target.y, getPosition().x, getPosition().y); // Distance from the target is the magnitude of the vector


    // If the distance is greater than 0, calculates the  steering (otherwise return zero vector)
    if (d > 0)
    {

        desired /= d; // Normalize desired
        // Two options for desired vector magnitude (1 -- based on distance, 2 -- maxspeed)
        if ((slowdown) && (d < 100.0f))
        {
            desired *= maxspeed * (d/100.0f); // This damping is somewhat arbitrary
        }
        else
        {
            desired *= maxspeed;
        }

        steer = desired - getVelocity();                              // Steering = Desired minus Velocity
        steer.x = ofClamp(steer.x, -maxforce, maxforce);    // Limit to maximum steering force
        steer.y = ofClamp(steer.y, -maxforce, maxforce);



        steer.x = (steer.x *.5)/2 ;
       // steer.y = (steer.y + 500)/2 ;

    }
    return steer;
}

// Definition of the method that draws the Boid boids
void Boid::draw()
{
	ofxBox2dRect::draw();
}

// Definition of the superclass Boid method that calculates the flocking behaviour of the Boid boids
void Boid::flock(vector<ofPtr<Boid>> boids)
{
    ofVec2f mou;
    int Buf1,Buf2;
    Buf1=200;
    Buf2=300;
    mou.set(Buf1,Buf2);
    ofVec2f sep = separate(boids);
    ofVec2f ali = align(boids);
    ofVec2f coh = cohesion(boids);

    // Arbitrarily weight of the forces
    sep *= 1.0;
    ali *= 1.0;
    coh *= 1.0;

    acc += sep + ali + coh;     // acceleration value
}

// definition of the superclass Boid method for separation
//the method checks for nearby Boid boids and steers away to avoid local crowding of the flockmates
ofVec2f Boid::separate(vector<ofPtr<Boid>> boids)
{
    float desiredseparation = 25.0f;
    ofVec2f steer;
    int count = 0;

    // For every boid in the system, check if it's too close
    for (int i = 0 ; i < boids.size(); i++)
    {
        Boid other = *boids.at(i);
        float d = ofDist(getPosition().x, getPosition().y, other.getPosition().x, other.getPosition().y);
        // If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
        if ((d > 0) && (d < desiredseparation))
        {
            // Calculates vector pointing away from neighbor
            ofVec2f diff = getPosition() - other.getPosition();
            diff /= d;			// normalize
            diff /= d;          // Weights by distance
            steer += diff;
            count++;            // Keeps track of how many
        }
    }
    // Average -- divide by how many
    if (count > 0)
    {
        steer /= (float)count;
    }
    // As long as the vector is greater than 0
    float mag = sqrt(steer.x*steer.x + steer.y*steer.y);
    if (mag > 0)
    {
        // Implement Reynolds: Steering = Desired - Velocity
        steer /= mag;
        steer *= maxspeed;
        steer -= getVelocity();
        steer.x = ofClamp(steer.x, -maxforce, maxforce);
        steer.y = ofClamp(steer.y, -maxforce, maxforce);
    }
    return steer;
}

// Definition of the superclass Boid method for alignment
//the method calculates the average velocity for every nearby Boid boid in the system
// steers towards the avarage direction on the nearby flockmates
ofVec2f Boid::align(vector<ofPtr<Boid>> boids)
{
    float neighbordist = 50.0;
    ofVec2f steer;
    int count = 0;
    for (int i = 0 ; i < boids.size(); i++)
    {
        Boid other = *boids.at(i);

        float d = ofDist(getPosition().x, getPosition().y, other.getPosition().x, other.getPosition().y);
        if ((d > 0) && (d < neighbordist))
        {
            steer += (other.getVelocity());
            count++;
        }
    }
    if (count > 0)
    {
        steer /= (float)count;
    }

    // As long as the vector is greater than 0
    float mag = sqrt(steer.x*steer.x + steer.y*steer.y);
    if (mag > 0)
    {
        // Implement Reynolds: Steering = Desired - Velocity
        steer /= mag;
        steer *= maxspeed;
        steer -= getVelocity();
        steer.x = ofClamp(steer.x, -maxforce, maxforce);
        steer.y = ofClamp(steer.y, -maxforce, maxforce);
    }
    return steer;
}

//definition ot the superclass Boid method for cohesion
//the method calculates the steering vector towards the average location of all nearby Boid boids
ofVec2f Boid::cohesion(vector<ofPtr<Boid>> boids)
{
    float neighbordist = 50.0;
    ofVec2f sum;   // Start with empty vector to accumulate all locations
    int count = 0;
    for (int i = 0 ; i < boids.size(); i++)
    {
        Boid other = *boids.at(i);
        float d = ofDist(getPosition().x, getPosition().y, other.getPosition().x, other.getPosition().y);
        if ((d > 0) && (d < neighbordist))
        {
            sum += other.getPosition(); // Add location
            count++;
        }
    }
    if (count > 0)
    {
        sum /= (float)count;
        return steer(sum, false);  // Steer towards the location
    }
    return sum;
}


void Boid :: UpdateLife() {
	age++;
	if(age>maxAge) isDead =true;
}