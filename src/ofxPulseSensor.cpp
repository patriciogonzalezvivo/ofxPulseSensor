//
//  ofxPulseSensor.cpp
//
//  Created by Patricio Gonzalez Vivo on 3/11/13.
//
//

#include "ofxPulseSensor.h"

void ofxPulseSensor::setup(){
    
#ifdef TARGET_RASPBERRY_PI
    
    //  Listen RaspberryPi GPIO's SPI on channel O
    //
    analogIn.setup(0);

    thresh = 512;
    T = 512;
    P = 512;
    amp = 100;
    firstBeat = true;
    secondBeat = false;
    Pulse = false;
    QS = false;
#else
    
    //  CONNECT TO ARDUINO
    //
    serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    string deviceLine, serialID;
    for(int i=0; i<deviceList.size();i++){
        deviceLine = deviceList[i].getDeviceName().c_str();
        if(deviceLine.substr(0,12) == "tty.usbmodem"){
            serialID = "/dev/"+deviceLine;
            cout<<"arduino serial = "<<serialID<<endl;
        }
    }
	serial.setup(serialID, 115200);
#endif
    BPM = 0;
    IBI = 600;
    bufferSize = 100;
}

void ofxPulseSensor::update(){

#ifdef TARGET_RASPBERRY_PI
    
    //  GET THE DATA FROM YOUR RaspberryPi GPIO
    //
    Signal = analogIn.value;
    
    
    if(data.size() > 10){
        int N = (ofGetElapsedTimef()-data[data.size()-1].sec)*1000;
        
        if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
            if (Signal < T){                        // T is the trough
                T = Signal;                         // keep track of lowest point in pulse wave
            }
        }
        
        if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
            P = Signal;                             // P is the peak
        }                                           // keep track of highest point in pulse wave
        
        //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
        // signal surges up in value every time there is a pulse
        //
        if (N > 250){                               // avoid high frequency noise
            
            if ( (Signal > thresh) && !Pulse && (N > (IBI/5)*3) ){
                Pulse = true;                       // set the Pulse flag when we think there is a pulse
                IBI = N;                            // measure time between beats in mS
                
                if(secondBeat){                     // if this is the second beat, if secondBeat == TRUE
                    secondBeat = false;             // clear secondBeat flag
//                    for(int i=0; i<=9; i++){        // seed the running total to get a realisitic BPM at startup
//                        rate[i] = IBI;
//                    }
                }
                
                if(firstBeat){                      // if it's the first time we found a beat, if firstBeat == TRUE
                    firstBeat = false;              // clear firstBeat flag
                    secondBeat = true;              // set the second beat flag
                    return;                         // IBI value is unreliable so discard it
                }
                
                // keep a running total of the last 10 IBI values
                //
                int runningTotal = 0;               // clear the runningTotal variable
                for(int i = data.size()-1; i>=data.size()-10; i--){
                    runningTotal += data[i].IBI;        // add up the 9 oldest IBI values
                }
                
                runningTotal += IBI;                // add the latest IBI to runningTotal
                runningTotal /= 10;                 // average the last 10 IBI values
                BPM = 60000/runningTotal;           // how many beats can fit into a minute? that's BPM!
            }
        }
        
        if (Signal < thresh && Pulse){      // when the values are going down, the beat is over
            Pulse = false;                          // reset the Pulse flag so we can do it again
            amp = P - T;                            // get amplitude of the pulse wave
            thresh = amp/2 + T;                     // set thresh at 50% of the amplitude
            P = thresh;                             // reset these for next time
            T = thresh;
        }
        
        if (N > 2500){                              // if 2.5 seconds go by without a beat
            thresh = 512;                           // set thresh default
            P = 512;                                // set P default
            T = 512;                                // set T default
            firstBeat = true;                       // set these to avoid noise
            secondBeat = false;                     // when we get the heartbeat back
        }
        
    }
#else
    
    //  GET THE DATA FROM ARDUINO
    //
    string serialIn;
    unsigned char buffer[1];
    while( serial.readBytes(buffer,1) > 0){
        if (buffer[0] == '\n'){
            break;
        }
        serialIn.push_back(buffer[0]);
    };
    
    if ( serialIn[0] == 'B' ){
        serialIn.erase(0,1);
        BPM = ofToInt(serialIn);
    }
    
    if ( serialIn[0] == 'Q' ){
        serialIn.erase(0,1);
        IBI = ofToInt(serialIn);
    }
    
    if ( serialIn[0] == 'S' ){
        serialIn.erase(0,1);
        Signal = ofToInt(serialIn);
        pushNewData();
    }
    
#endif
    
    cleanData();
}

void ofxPulseSensor::pushNewData(){
    PulseData newPack;
    newPack.sensor = Signal;
    newPack.BPM = BPM;
    newPack.IBI = IBI;
    newPack.val = ofMap(Signal, 212, 1024, -1.0, 1.0, true);
    newPack.sec = ofGetElapsedTimef();
    
    if ((Signal > 212)&&(Signal < 1023)){
        newPack.bGoodData = true;
    } else {
        newPack.bGoodData = false;
    }
    
    data.push_back(newPack);
}

PulseData ofxPulseSensor::getData(const float &_sec){
    
    PulseData pData;
    
	for (int i = 0; i < data.size()-1; i++){
		if (_sec >= data[i].sec && _sec <= data[i+1].sec){
			
			// calculate pct:
			float part = _sec - data[i].sec;
			float whole = data[i+1].sec - data[i].sec;
			float pct = part / whole;
			
            pData.sensor = (1.0-pct) * (float)data[i].sensor + (pct) * (float)data[i+1].sensor;
            pData.BPM = (1.0-pct) * (float)data[i].BPM + (pct) * (float)data[i+1].BPM;
            pData.IBI = (1.0-pct) * (float)data[i].IBI + (pct) * (float)data[i+1].IBI;
            
            pData.val = (1.0-pct) * (float)data[i].val + (pct) * (float)data[i+1].val;
            pData.sec = (1.0-pct) * (float)data[i].sec + (pct) * (float)data[i+1].sec;
    
            break;
		}
	}
    
    return pData;
}

float ofxPulseSensor::getVal(const float &_sec){
    
	for (int i = 0; i < data.size()-1; i++){
		if (_sec >= data[i].sec && _sec <= data[i+1].sec){
			
			// calculate pct:
			float part = _sec - data[i].sec;
			float whole = data[i+1].sec - data[i].sec;
			float pct = part / whole;
			
            return (1.0-pct) * (float)data[i].val + (pct) * (float)data[i+1].val;
		}
	}
    
}

int ofxPulseSensor::getBPM(const float &_sec){
    for (int i = 0; i < data.size()-1; i++){
		if (_sec >= data[i].sec && _sec <= data[i+1].sec){
			
			// calculate pct:
			float part = _sec - data[i].sec;
			float whole = data[i+1].sec - data[i].sec;
			float pct = part / whole;
			
            return (1.0-pct) * (float)data[i].BPM + (pct) * (float)data[i+1].BPM;
		}
	}
}

int ofxPulseSensor::getIBI(const float &_sec){
    for (int i = 0; i < data.size()-1; i++){
		if (_sec >= data[i].sec && _sec <= data[i+1].sec){
			
			// calculate pct:
			float part = _sec - data[i].sec;
			float whole = data[i+1].sec - data[i].sec;
			float pct = part / whole;
			
            return (1.0-pct) * (float)data[i].IBI + (pct) * (float)data[i+1].IBI;
		}
	}
}

int ofxPulseSensor::getSensor(const float &_sec){
    for (int i = 0; i < data.size()-1; i++){
		if (_sec >= data[i].sec && _sec <= data[i+1].sec){
			
			// calculate pct:
			float part = _sec - data[i].sec;
			float whole = data[i+1].sec - data[i].sec;
			float pct = part / whole;
			
            return (1.0-pct) * (float)data[i].sensor + (pct) * (float)data[i+1].sensor;
		}
	}
}

void ofxPulseSensor::cleanData(){
    
    //  Check last values trying to clean wrong numbers
    //
    if (data.size() > 6){
        //  Search for the last good data
        //
        int lastGoodData = 0;
        int lastGoodDataIndex = 0;
        float lastGoodDataTime = 0.0;
        for (int i = data.size()-1; i >= 0; i--){
            
            if (data[i].bGoodData){
                lastGoodData = data[i].sensor;
                lastGoodDataIndex = i;
                lastGoodDataTime = data[i].sec;
            } else if ( lastGoodData != 0 ){
                //  Search for the next good Data
                //
                int nextGoodData = 0;
                int nextGoodDataIndex = 0;
                float nextGoodDataTime = 0.0;
                for (int j = i-1; j >= 0 ; j-- ){
                    if (data[j].bGoodData){
                        nextGoodData = data[j].sensor;
                        nextGoodDataIndex = j;
                        nextGoodDataTime = data[j].sec;
                        break;
                    }
                }
                
                if (nextGoodData != 0){
                    
                    //  Try to found a value that have sense given the others
                    //
                    //                    float pct = ofMap(i, nextGoodDataIndex, lastGoodDataIndex, 0.0, 1.0,true);
                    float pct = ofMap(i, nextGoodDataTime, lastGoodDataTime, 0.0, 1.0,true);
                    
                    data[i].sensor = nextGoodData*(1.0-pct)+lastGoodData*pct;
                    data[i].val = ofMap(data[i].sensor, 212, 1023, -1.0, 1.0,true);
                    data[i].bGoodData = true;
                }
                
                break;
            }
            
        }
    }
    
    if (bufferSize >= 0){
        while (data.size() > bufferSize) {
            data.erase(data.begin());
        }
    }
}

void ofxPulseSensor::draw(){
    
    ofNoFill();
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
    
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    ofSetColor(255);
    
    if ( data.size() > 0){
        if (data[data.size()-1].bGoodData){
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(0,data[data.size()-1].val*ofGetHeight()*0.5);
            
            ofFill();
            ofCircle(0,0, 2);
            
            ofDrawBitmapString("Signal: "+ofToString(Signal),0,15);
            ofDrawBitmapString("BPM: "+ofToString(BPM),5,30);
            ofDrawBitmapString("IBI: "+ofToString(IBI),5,45);
            
            ofPopMatrix();
            ofPopStyle();
        }
    }
    
    float x = 0;
    for(int i = data.size()-1; i >= 0 && i >= data.size()-ofGetWidth()*0.5 ; i-- ){
        
        if ( data[i].bGoodData ){
            float alpha = ofMap(x, -ofGetWidth()*0.5, 0.0, 0.0, 1.0);
            mesh.addColor( ofFloatColor(1.0, alpha) );
            mesh.addVertex(ofPoint(x, data[i].val*ofGetHeight()*0.5));
        }
        
        x--;
    }
    
    mesh.draw();
    ofPopMatrix();
}