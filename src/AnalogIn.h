//
//  AnalogIn.h
//
//  Created by Patricio Gonzalez Vivo on 4/26/13.
//  Based on by jason van cleave on 3/9/13 wich is a translation of http://learn.adafruit.com/reading-a-analog-in-and-controlling-audio-volume-with-the-raspberry-pi/connecting-the-cobbler-to-a-mcp3008
//  and http://raspberrypihobbyist.blogspot.com/2012/12/analog-interface.html
//

#pragma once

#include "ofMain.h"

#ifdef TARGET_RASPBERRY_PI
#include "wiringPiSPI.h"
#endif

class AnalogIn: public ofThread {
public:
	AnalogIn();
    
	bool    setup(int _channel = 0);
	
	int     lastValue;
	int     value;
	int     changeAmount;
	
	bool    doToleranceCheck;
	int     tolerance;
    
private:
    void    threadedFunction();
	int     readAnalogDigitalConvertor();
    
    int     input;
    int     channel;
    bool    didChange;
	bool    isReady;
};


