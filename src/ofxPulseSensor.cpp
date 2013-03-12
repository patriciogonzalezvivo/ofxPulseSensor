//
//  ofxPulseSensor.cpp
//  05_Pulse_Wave
//
//  Created by Patricio Gonzalez Vivo on 3/11/13.
//
//

#include "ofxPulseSensor.h"

ofxPulseSensor::ofxPulseSensor(){    
}

void ofxPulseSensor::setup(int _dev, int _baud){
	serial.setup(0, _baud);
}

void ofxPulseSensor::update(){
    string serialIn;
    
    unsigned char buffer[1];
    while( serial.readBytes( buffer, 1) > 0){
        if (buffer[0] == '\n'){
            break;
        }
        serialIn.push_back(buffer[0]);
    };
    
//    cout << serialIn << endl;
    
    if ( serialIn[0] == 'S' ){
        
        serialIn.erase(0,1);
        Sensor = ofToInt(serialIn);
        
        if ( (Sensor > 212) && (Sensor < 1023)){
            float tmpRaw = ofMap(Sensor, 212, 1023, -1.0, 1.0);
            raw.push_back( tmpRaw );
        }
        
    } else if ( serialIn[0] == 'B' ){
        serialIn.erase(0,1);
        BPM = ofToInt(serialIn);
        rate.push_back(BPM);
        
    } else if ( serialIn[0] == 'Q' ){
        serialIn.erase(0,1);
        IBI = ofToInt(serialIn);
    }
    
    while (raw.size() > 1000) {
        raw.erase(raw.begin());
    }
    
    while (rate.size() > 1000) {
        rate.erase(rate.begin());
    }
}