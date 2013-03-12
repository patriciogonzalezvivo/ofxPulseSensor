#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    pSensor.setup(0, 115200);
}



//--------------------------------------------------------------
void testApp::update(){
    pSensor.update();
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(0);
    
    ofDrawBitmapString("Sensor: " + ofToString(pSensor.Sensor), 15,15);
    ofDrawBitmapString("BPM: " + ofToString(pSensor.BPM), 15,30);
    ofDrawBitmapString("IBI: " + ofToString(pSensor.IBI), 15,45);
    
    ofNoFill();
    
    ofPushMatrix();
    ofTranslate(0, ofGetHeight()*0.5);
    
    ofBeginShape();
    for(int i = 0; i < pSensor.raw.size();i++ ){
        ofVertex(i, pSensor.raw[i]*ofGetHeight()*0.5);
    }
    ofEndShape();
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}