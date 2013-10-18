//
//  ofxPulseSensor.h
//
//  Created by Patricio Gonzalez Vivo on 3/11/13.
//
#pragma once

#include "ofMain.h"

#ifdef TARGET_RASPBERRY_PI
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

    int         Signal;     //  Raw PulseSensor Signal
    float       Value;      //  Normailzed PulseSensor Signal (-1 to 1)
    int         IBI;        //  http://en.wikipedia.org/wiki/Interbeat_interval
    int         BPM;        //  Normal between 60Ð100 bpm ( http://en.wikipedia.org/wiki/Heart_rate )
    
private:
    void        pushNewData();
    void        calculateBPM();
    void        cleanData();
    
#ifdef TARGET_RASPBERRY_PI
    AnalogIn    analogIn;
#else
    ofSerial    serial;
#endif
    
    vector<PulseData>   data;
    int         bufferSize;
    
    float       lastPulse;
    vector<int> rate;
};
