#pragma once

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxOscParameterSync.h"
#include "ofxArtnet.h"

#define WIDTH 1024
#define HEIGHT 768

#define PLAYERPORT 6000
#define CONTROLERPORT 7000
//#define HOSTNAMEPLAYER "localhost"
#define HOSTNAMEPLAYER "Pierres-MacBook-Pro-2.local"

using namespace std;


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
    
        ofVideoPlayer mPlayer;
    ofVec2f videoSize;
    int videoChannels;
    
    bool guiON;
    ofxPanel gui;
    ofParameterGroup LedDisplay;
    float spacingSlider;
    ofParameter < bool > displayLeds;
    ofParameter < bool > playVideo;
    ofParameter < string > Node_IP;
    ofParameter < string > nameVideo;
    ofParameter < bool > rgbColors;
    void sendingAmount( int& newSend);
    void loadingNewNode_IP(string& newNode_IP);
    ofxOscParameterSync sync;

    
    // artnet pixels
    
    ofxArtnetSender artnetNode_IP;
    void sendArtnet(ofPixels & pixels);
    ofPixels dataToSend;
    ofFbo f;
    
    
    string IPAddress;
    unsigned int net;
    unsigned int subnet;
    int amntUniverses;
    vector < unsigned int > universe;
    unsigned int port;
    
    
    void loadVideo();
    std::string videoName;
    void loadingNewVideo(string& newName);
        
        

};
