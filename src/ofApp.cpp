///////////////////////////////////////////////////////
///// Programming with Structures Module 6 CreaTe /////
/////  Created by Peter Verzijl and Gege Zhang    /////
/////                  2015                       /////
///////////////////////////////////////////////////////

#include "ofApp.h"
#include "flock\FishOne.h"
#include "flock\FishTwo.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);			// Set graphics so it runs smoothly
//	ofSetLogLevel(OF_LOG_NOTICE);		// Debug level

	kinect.setRegistration(true);		// Registrate depth checking

	kinect.init(false, false);			// Disables video image (faster fps)
	kinect.open();						// Opens first available kinect

	kinectAngle = 10;					// Level kinect at startup
	kinect.setCameraTiltAngle(kinectAngle);

	// Debug kinect startup messages
	// Gege's computer lagging
	/*
	if (kinect.isConnected()) 
	{
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
	*/

	// Populate and init image with correct image size
	grayImage.allocate(kinect.width, kinect.height);

	useThreshold = true;				// Tell openCV to use threshold values
	nearThreshold = 255;				// Check as near as possible
	farThreshold = 243;					// Optimal for fist tracking

	// Set box2d parameters
	box2d.init();
	box2d.setGravity(0, 0);				// Set a vector2 for the gravity to 0 initially
	box2d.createBounds();				// Create the bounds of the box2d world
	box2d.setFPS(30.0);					// Set the physics update rate

	// Add two 'hands'
		
	// Player left
	playerLeft = ofPtr<Boid>(new Boid);
	playerLeft->setPhysics(3.0, 0.53, 0.1);
	playerLeft->setup(box2d.getWorld(), ofGetWidth() * 0.25f, ofGetHeight() * 0.5f, 120, 50);
	playerLeft->setFixedRotation(true);
	playerLeft->setRotationFriction(1000);
	playerLeft->setRotation(0);
	boids.push_back(playerLeft);

	// Player right
	playerRight = ofPtr<Boid>(new Boid);
	playerRight->setPhysics(3.0, 0.53, 0.1);
	playerRight->setup(box2d.getWorld(), ofGetWidth() * 0.75f, ofGetHeight() * 0.5f, 120, 50);
	playerRight->setFixedRotation(true);
	playerRight->setRotationFriction(1000);
	playerRight->setRotation(0);
	boids.push_back(playerRight);
	

	//generating new fishes (20)
	for (int i = 0; i < 20; i++)
	{
		boids.push_back(ofPtr<FishOne>(new FishOne));
		boids.back().get()->setPhysics(3.0, 0.53, 0.1);
		boids.back().get()->setup(box2d.getWorld(), ofRandomWidth(), 50, 20, 20);
	}

	ofSetFrameRate(60);					// Try to keep the display framerate at 60fps
}

//--------------------------------------------------------------
void ofApp::update(){

	kinect.update();					// Update kinect first to get images

	if (kinect.isFrameNew())			// Update code only when kinect has new data
	{
		// Load greyscale depth images
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		grayImage.resize(ofGetWidth(), ofGetHeight());	// Set the image so that it covers the whole screen
		grayImage.mirror(false, true);	// For some reason the image is mirrored :S so we mirror it back!
		
		// Do treshold!
		unsigned char *pixels = grayImage.getPixels();
		int numPixels = grayImage.getWidth() * grayImage.getHeight();
		for (int i = 0; i < numPixels; i++)
		{
			// Change the pixels to white and black so we can see them better later
			pixels[i] = (pixels[i] < nearThreshold && pixels[i] > farThreshold) ? 255 : 0;
		}

		grayImage.flagImageChanged();	// Tells open CV that the image was updated

		// Find contours
		contourFinder.findContours(grayImage, 30, (kinect.width * kinect.height) * 0.5f, 20, false); 
	}

	// Now we have the blobs from findContours, iterate trough them
	if (contourFinder.nBlobs > 0 && boids.size() > 0) {
		ofxCvBlob blob = contourFinder.blobs.at(0);
		ofVec2f blobPos = ofVec2f(blob.centroid.x, blob.centroid.y);
		
		if (blobPos.x < ofGetWidth() * 0.5f) 
		{
			float distance = playerLeft->getPosition().distance(blobPos);
			ofVec2f force = (blobPos - playerLeft->getPosition()).normalized();
			playerLeft->setVelocity(force * 0.1f * distance);
			playerLeft->setRotation(0);
		}
		else
		{
			float distance = playerRight->getPosition().distance(blobPos);
			ofVec2f force = (blobPos - playerRight->getPosition()).normalized();
			playerRight->setVelocity(force * 0.1f * distance);
			playerRight->setRotation(0);
		}
	}

	// Update movement of boxes
	for (int i=0; i<boids.size(); i++) 
	{
		ofPtr<ofxBox2dRect> boid = boids.at(i);
		boid->update();
		if (boid->getPosition().y < ofGetHeight() * 0.5f)
		{
			boid->addForce(ofVec2f(0, 30), 1.0f);
		}
		else
		{
			boid->friction = 10.0f;
		}
	}

	// After everything is done
	box2d.update();

	//Killing the fishes
	explode.loadSound("explosion.wav");
	int deadFishNum =100;												//setting number if no fishes are left -> end destroying more fishes to prevent error

	for (int l = 0 ; l < boids.size(); l++)						//loop through all the fishes
	{
		ofPtr<Boid> fish = boids.at(l);							//set ofPointer fish equal the "i" one in the loop

		if(fish->getPosition().y < ofGetHeight() * 0.5f)				//if the specific fish is on the top half of the screen
		{
			fish->UpdateLife();											//run update life (lifespan --)
			deadFishNum = l;											//save the array number of the "i"-th fish in deadFishNum
		}

		if(deadFishNum != 100 && fish->isDead)							//erase fish at i if not all fishes are killed and if boolean isDead is true.
		{
			boids.erase(boids.begin()+deadFishNum);			//erase that fish
			explode.play();
		}			
		else{ /* cout<< "I KILLED THEM ALL! ";*/}						//something could happen if al the fishes are killed. 
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(150);										// Set default background color a nice shade of gray

	//grayImage.draw(0, 0, grayImage.width, grayImage.height);		// Draw threshold image
	//contourFinder.draw(0, 0, grayImage.width, grayImage.height);	// Draw the found contours

	// Draw players
	ofFill();
	ofSetColor(0, 0, 255);
	playerLeft->draw();
	ofSetColor(255, 0, 0);
	playerRight->draw();

	// Draw box2D
	ofSetColor(255, 255, 255);
	for (int i = 2; i < boids.size(); i++) {
		boids.at(i)->draw();
	}
	
	// Draw water
	ofSetColor(0, 0, 255, 150); 
	ofRect(0, ofGetHeight() * 0.5f, ofGetWidth(), ofGetHeight() * 0.5f);

	// draw the ground
	box2d.drawGround();

	ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 10,15);
}

//--------------------------------------------------------------
void ofApp::exit() {

	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
