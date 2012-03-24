#pragma once

#include "ofMain.h"
#include "ofxCUPS.h"

class testApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    void keyPressed  (int key);
    
    // required adding [libcups.2.dylib]
    ofxCUPS printer;
    
};
