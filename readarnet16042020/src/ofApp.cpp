#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
       ofBuffer buffer = ofBufferFromFile("ArtNet/ports.txt");
       
       if(buffer.size()) {

           for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {

               string line = *it;
               
               if(line.empty() == false) {
                   if(line == "Port"){
                       line = *(++it);
                       port = std::stoi(line);
                       cout << "Port = " << ofToString(port) << endl;
                       
                   }
                   if(line == "Net"){
                       line = *(++it);
                       net = std::stoi(line);
                       cout << "Net = " << ofToString(net) << endl;
                   }
                   if(line == "Subnet"){
                       line = *(++it);
                       subnet = std::stoi(line);
                       cout << "Subnet = " << ofToString(subnet) << endl;
                   }
                   if(line == "AmntUniverse"){
                       line = *(++it);
                       amntUniverses = std::stoi(line);
                       cout << "amntUniverses = " << ofToString(amntUniverses) << endl;
                       for( int i = 0; i < amntUniverses; i++){
                           universe.push_back(i);
                       }
                   }
               }
           }
       }
    
    
    videoName = "video.mov";
    loadVideo();
    
    guiON = false;
    gui.setup("Controls");
    LedDisplay.setName("Led Matrix");
    LedDisplay.add(Node_IP.set("Node_IP", "10.0.0.10"));
    LedDisplay.add(displayLeds.set("display LEDs", true));
    LedDisplay.add(playVideo.set("play video", true));
    LedDisplay.add(nameVideo.set("nameVideo", "rgb.mov"));
    LedDisplay.add(rgbColors.set("couleurs", false));
    
    spacingSlider = 5.5;
    
    ofSetFrameRate(20);
    gui.add(LedDisplay);
    
    nameVideo.addListener(this, &ofApp::loadingNewVideo);
    
    Node_IP.addListener(this, &ofApp::loadingNewNode_IP);
    
    sync.setup((ofParameterGroup&)gui.getParameter(),PLAYERPORT, HOSTNAMEPLAYER, CONTROLERPORT);

    //f.clear();
    
    // buffer de 180 pixels de haut: pas dynamique??
   // f.allocate(1, 180, GL_RGB);
    gui.loadFromFile("settings.xml");


}
void ofApp::loadingNewVideo(string & newVideoName){
    videoName = newVideoName;
    loadVideo();
}

//--------------------------------------------------------------
void ofApp::loadingNewNode_IP(string& newNode_IP){
    Node_IP = newNode_IP;
    cout << Node_IP << endl;
 
    artnetNode_IP.disableThread();
    artnetNode_IP.setup(Node_IP, port);
 
}

//--------------------------------------------------------------
void ofApp::loadVideo(){
    
    std::string fullPathVideo = "videos/" + videoName;
    cout << fullPathVideo << endl;
    mPlayer.load(fullPathVideo);
    mPlayer.setLoopState(OF_LOOP_NORMAL);
    mPlayer.play();
    
    ofPixels & pixels = mPlayer.getPixels();
    videoSize.x = pixels.getWidth();
    videoSize.y = pixels.getHeight();
    videoChannels = pixels.getNumChannels();
    cout << videoSize.y << " " << videoSize.x << endl;
    
    f.clear();
    f.allocate(1, videoSize.y, GL_RGB);
    
    cout << "f.width = " << f.getWidth() << " f.height = " << f.getHeight() << endl;
}

//--------------------------------------------------------------
void ofApp::sendingAmount( int& newSend){
    
    
    f.clear();
    f.allocate(1, newSend, GL_RGB);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    sync.update();
    
    if(playVideo)mPlayer.update();
    sendArtnet(mPlayer.getPixels());
    
    if(!playVideo){
        mPlayer.stop();
    }else{
        
        mPlayer.play();
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(playVideo)mPlayer.draw(0,0);
    
    ofPixels & pixels = mPlayer.getPixels();
    
       int offsetWidth = (WIDTH - videoSize.x * spacingSlider)/ 2;
       int offsetHeight = (HEIGHT - videoSize.y * spacingSlider)/ 2;
  
    if(displayLeds){
        for (int i = 0; i < videoSize.x; i += 1){
            for (int j = 0; j < videoSize.y; j += 1){
                unsigned char r = pixels[(j * videoSize.x + i)*videoChannels];
                unsigned char g,b;
                
                g = b = r;
                
                if(videoChannels > 1 && rgbColors){
                    
                    g = pixels[(j * videoSize.x + i)*videoChannels + 1];
                    b = pixels[(j * videoSize.x + i)*videoChannels + 2];
                }
                
                float val = 1 - ((float)r / 255.0f);
                ofColor fColor = ofColor(r, g, b);
                ofPushStyle();
                ofSetColor(fColor);
                //taille des pixels à l'écran
                ofDrawRectangle(offsetWidth + i * spacingSlider, offsetHeight + j * spacingSlider, spacingSlider * .8, spacingSlider  * .8);
                ofPopStyle();
            }
        }
    }
    
    
    
    // GUI
    if(guiON)gui.draw();

}

//--------------------------------------------------------------
void ofApp::sendArtnet(ofPixels & pixels){
    
    for (int i = 0; i <videoSize.x ; i += 1){    
        f.begin();
        ofClear(0);
        for (int j = 0; j < videoSize.y; j += 1){
            unsigned char r = pixels[(j * videoSize.x + i)*videoChannels];
            unsigned char g,b;
            
            g = b = r;
            
            if(videoChannels > 1 && rgbColors){
                g = pixels[(j * videoSize.x + i)*videoChannels + 1];
                b = pixels[(j * videoSize.x + i)*videoChannels + 2];
            }
           
           //on récupère le pixel en RGB
            ofColor fColor = ofColor(r, g, b);
            ofPushStyle();
            ofSetColor(fColor);
            
            //on rempli le buffer
            ofDrawRectangle(0,j,1,1);
            ofPopStyle();
        }
        
        f.end();
        f.readToPixels(dataToSend);
        ofxArtnetMessage m(dataToSend);
       // if(i<16)
            
        //{   //m.setUniverse(0, 1, i+1);
            m.setUniverse(0, 0, i);//subnet,net, univers
            artnetNode_IP.sendArtnet(m);
        //}
        //else{
            //si tu veux tes 32 univers sur la meme ip
            //m.setUniverse(0, 1, i-15);
          //  m.setUniverse(0, 0, i-16);
           // artnetNode_IP.sendArtnet(m);
        //}
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'g')guiON = !guiON;
}
