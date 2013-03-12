//
//  ofxPulseSensor.h
//  05_Pulse_Wave
//
//  Created by Patricio Gonzalez Vivo on 3/11/13.
//
//

#ifndef PULSSENSOR
#define PULSSENSOR

#include "ofMain.h"

class ofxPulseSensor {
public:
    
    ofxPulseSensor();
    
    void        setup(int _dev, int _baud);
    void        update();
    
    int         Sensor;     // HOLDS PULSE SENSOR DATA FROM ARDUINO
    int         IBI;        // HOLDS TIME BETWEN HEARTBEATS FROM ARDUINO
    int         BPM;        // HOLDS HEART RATE VALUE FROM ARDUINO
    
    vector<float>   raw;        // HOLDS HEARTBEAT WAVEFORM DATA BEFORE SCALING
    vector<int>     rate;       // USED TO POSITION BPM DATA WAVEFORM
    
private:
//    bool        readUntil(string& rResult, char cUntil);
    
    ofSerial    serial;
    
    
};
#endif
