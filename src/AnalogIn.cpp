//
//  AnalogIn.cpp
//  lumiereCam
//
//  Created by Patricio Gonzalez Vivo on 4/26/13.
//
//

#include "AnalogIn.h"

AnalogIn::AnalogIn(){
	isReady      = false;
    didChange    = false;
	changeAmount = 0;
	lastValue    = 0;
	value        = 0;
    channel      = 0;
    
    //10k trim pot connected to adc #0
    input = 0;
}


bool AnalogIn::setup(int _channel){
    
    channel     = _channel;
    isReady     = false;
	int status  = -1;
    
#ifdef TARGET_RASPBERRY_PI
    status  = wiringPiSPISetup(channel, 1000000);
#endif
    
	if (status != -1){
		ofLogVerbose() << "wiringPiSetup PASS";
		isReady = true;
	} else {
		ofLogError() << "wiringPiSetup FAIL status: " << status;
	}
    
	if (isReady) {
		startThread(false, true);
	}
    
	return isReady;
}


void AnalogIn::threadedFunction(){
	while ( isThreadRunning() ){
		didChange = false;
		
		value = readAnalogDigitalConvertor();
		changeAmount = abs(value - lastValue);
        
		if(changeAmount!=0) {
            didChange = true;
        }
        
		lastValue = value;
        sleep(10);
	}
}

int  AnalogIn::readAnalogDigitalConvertor() {
	uint8_t buffer[3];
    
	buffer[0] = 1;
	buffer[1] = (8+input)<<4;
	buffer[2] = 0;
    
#ifdef TARGET_RASPBERRY_PI
	wiringPiSPIDataRW(channel, buffer, 3);
#endif
    
	return ((buffer[1]&3) << 8) + buffer[2];
}