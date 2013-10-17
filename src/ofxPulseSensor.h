//
//  ofxPulseSensor.h
//
//  Created by Patricio Gonzalez Vivo on 3/11/13.
//
#pragma once

#include "ofMain.h"

#ifdef TARGET_RASPBERRY_PI
#include "wiringPiSPI.h"
#include "AnalogIn.h"
#endif

struct PulseData{
    int     sensor;
    int     IBI;
    int     BPM;
    
    float   val;
    float   sec;
    
    bool    bGoodData;
};

class ofxPulseSensor{
public:
    
    void        setup();
    
    PulseData   getData(const float &_sec);
    float       getVal(const float &_sec);
    int         getBPM(const float &_sec);
    int         getIBI(const float &_sec);
    int         getSensor(const float &_sec);
    
    void        update();
    void        draw();

    int         Signal;
    int         IBI;        // http://en.wikipedia.org/wiki/Heart_rate
    int         BPM;        // http://en.wikipedia.org/wiki/Interbeat_interval
    
private:
    void        pushNewData();
    void        cleanData();
    
#ifdef TARGET_RASPBERRY_PI
    AnalogIn    analogIn;
    int         thresh;
    int         T;
    int         P;
    int         amp;
    bool        firstBeat;
    bool        secondBeat;
    bool        Pulse;
    bool        QS;
#else
    ofSerial    serial;
#endif
    
    vector<PulseData>   data;
    int         bufferSize;
};
