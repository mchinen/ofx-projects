//-------------------------------------------------------------------------------------
//
// \file bTracker.cpp
// \brief 
//
// \author Francois Visconte <f.visconte@gmail.com>
// http://digitalork.blogspot.com, http://github.com/dopuskh3
//
//------------------------------------------------------------------------------------ 



#include "bTracker.h"
#include "bBlob.h"

//-------------------------------------------------------------------------
void bTracker::setup(int w, int h){ 
     
	this->vidGrabber.setVerbose(true);
    // size 
    this->height=h; 
    this->width =w; 
    
    // initGrabber
	vidGrabber.initGrabber(this->width, this->height); 
    // contour finder 

    // allocate images    
    this->source.allocate(w, h);
    this->threshold.allocate(w, h); 
    this->grey.allocate(w, h);
    this->background.allocate(w, h); 

    // set default params
    this->threshValue = 30; 
    this->diffMode  = DARK_MODE; 
    this->dilate=2; 
    this->erode=2;
    this->blur=2;

    this->minBlobSize = w*h*0.1; 
    this->maxBlobSize = w*h*0.75;
    this->maxBlobs = 10; 

    this->setBackground(); 	
  
    this->particleSystem = new ParticleSystem(100); 
}

//------------------------------------------------------------------------- 
void bTracker::update(){
  // grab frame
  this->vidGrabber.grabFrame();
  if (this->vidGrabber.isFrameNew()){
    unsigned char * pixels = this->vidGrabber.getPixels();
    this->source.setFromPixels(pixels, this->width, this->height); 
  }

  // convert to greyscale
  this->grey = this->source; 
  
  switch(this->diffMode){
    case ABDIFF_MODE:
      this->threshold = this->grey; 
      this->threshold.absDiff(this->background); 
      break; 
    case LIGHT_MODE:
      this->threshold = this->grey; 
      this->threshold -= this->background; 
      break; 
    default: // case DARK_MODE:
      this->threshold = this->background; 
      this->threshold -= this->grey; 
      break; 
  }
  
  this->threshold.threshold(this->threshValue); 
  //this->threshold.adaptiveThreshold(20, this->threshValue, 0, 1);   
 
  //  erode 
  for(int i=0; i < this->erode; i++ )
    this->threshold.erode_3x3(); 
  
  // dilate 
  for(int i=0; i < this->dilate; i++)
	this->threshold.dilate_3x3(); 
 // blur  
  for(int i=0; i < this->blur; i++)
    this->threshold.blur();
  
  int numFound = contourFinder.findContours(this->threshold, this->minBlobSize, this->maxBlobSize, this->maxBlobs, false, true);
  //contourFinder.findConvexHulls();
  contourFinder.smoothApproxChains();
   
}

//------------------------------------------------------------------------- Set threshold
void bTracker::setThreshold(int t){ this->threshValue = t; }
//------------------------------------------------------------------------- set background
void bTracker::setBackground(){ this->background = this->source; }
//------------------------------------------------------------------------- Set diff mode
void bTracker::setDiffMode(int diffMode){
  this->diffMode = diffMode; 
} 

//-------------------------------------------------------------------------  Retrieve background image
ofxCvGrayscaleImage *bTracker::getBackgroundImage(){ return &this->background; } 

//------------------------------------------------------------------------- retrieve current image
ofxCvGrayscaleImage *bTracker::getThresholdImage(){ return &this->threshold; } 

//------------------------------------------------------------------------- get frame
ofxCvColorImage *bTracker::getFrameImage(){  return &this->source; }

//------------------------------------------------------------------------- 
// return blobs 
vector<ofxCvBlob>  &bTracker::getBlobs(){
  return contourFinder.blobs; 
}
//-------------------------------------------------------------------------
//Return convex blobs  
vector<ofxCvBlob> &bTracker::getConvexBlobs(){
  return contourFinder.convexBlobs; 
}

//------------------------------------------------------------------------- 
void bTracker::draw(float x, float y, float h,float  w){
  ofSetColor(0,0,255);
  contourFinder.draw(x, y, h, w); 
}


