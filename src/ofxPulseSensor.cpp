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
    bufferSize = 500;
    
    lastPulse = 0;
}

void ofxPulseSensor::update(){

#ifdef TARGET_RASPBERRY_PI
    
    //  GET THE DATA FROM YOUR RaspberryPi GPIO
    //
    Signal = analogIn.value;
    pushNewData();
    
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
        //  Un comment to check your parameters
        //
//        cout << "Arduino BPM " << ofToInt(serialIn) << endl;
//        BPM = ofToInt(serialIn);
    }
    
    if ( serialIn[0] == 'Q' ){
        serialIn.erase(0,1);
        //  Un comment to check your parameters
        //
//        cout << "Arduino IBI " << ofToInt(serialIn) << endl;
//        IBI = ofToInt(serialIn);
    }
    
    if ( serialIn[0] == 'S' ){
        serialIn.erase(0,1);
        Signal = ofToInt(serialIn);
        pushNewData();
    }
    
#endif
    
    cleanData();
    calculateBPM();
}

void ofxPulseSensor::pushNewData(){
    Value = ofMap(Signal, 212, 1024, 1.0, -1.0, true);
    
    PulseData newPack;
    newPack.sensor = Signal;
    newPack.BPM = BPM;
    newPack.IBI = IBI;
    newPack.val = Value;
    newPack.sec = ofGetElapsedTimef();
    
    if ((Signal > 212)&&(Signal < 1023)){
        newPack.bGoodData = true;
    } else {
        newPack.bGoodData = false;
    }
    
    data.push_back(newPack);
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
                    data[i].val = ofMap(data[i].sensor, 212, 1023, 1.0, -1.0,true);
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

void ofxPulseSensor::calculateBPM(){
    
    if (data.size() > 10){
        //  Mesure the Min and Max values of the last 2.5 sec
        //
        float minVal = 1;
        float maxVal = -1;
        float now = ofGetElapsedTimef();
        for(int i = data.size()-2; i >= 0; i--){
            if ( now - data[i].sec < 1.0 ){
                if(data[i].val<minVal){
                    minVal=data[i].val;
                } else if(data[i].val>maxVal){
                    maxVal=data[i].val;
                }
            } else {
                break;
            }
        }
        float medVal = (minVal+maxVal)*0.5;
        
        //  Zero-crossing
        //
        float actual = data[ data.size()-1 ].val-medVal;
        float prev = data[ data.size()-2 ].val-medVal;
        if (prev < 0 && actual >= 0) {
            int diff = (now-lastPulse)*1000; //MilliSeconds
            if (diff > 250){
                IBI = diff;
                rate.push_back(IBI);
                lastPulse = now;
            }
        }
        
        while (rate.size()>10) {
            rate.erase(rate.begin());
        }
        
        if ( rate.size() == 10){
            int runningTotal = 0;
            for(int i=0; i<rate.size(); i++){
                runningTotal += rate[i];
            }
            
            runningTotal /= rate.size();
            BPM = 60000/runningTotal;
        }
    }
}

void ofxPulseSensor::draw(){
    
    ofDrawBitmapString("Signal: "+ofToString(Signal),0,15);
    ofDrawBitmapString("BPM: "+ofToString(BPM),5,30);
    ofDrawBitmapString("IBI: "+ofToString(IBI),5,45);
    
    ofPushStyle();
    ofPushMatrix();
    
    if ( data.size() > 0){
        ofPushMatrix();
        ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
        ofSetColor(255);
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
        float x = 0;
        for(int i = data.size()-1; i >= 0; i-- ){
            
            if ( data[i].bGoodData ){
                float alpha = ofMap(x, data.size(), 0.0, 0.0, 1.0);
                mesh.addColor( ofFloatColor(1.0, alpha) );
                mesh.addVertex(ofPoint(x, data[i].val*ofGetHeight()*0.5));
            }
            
            x--;
        }
        
        ofNoFill();
        mesh.draw();
        
        if (data[data.size()-1].bGoodData){
            ofFill();
            ofCircle(0,data[data.size()-1].val*ofGetHeight()*0.5, 2);
        }
        
        ofPopMatrix();
    }
    ofPopMatrix();
    ofPopStyle();
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