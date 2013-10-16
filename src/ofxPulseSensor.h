//
//  ofxPulseSensor.h
//
//  Created by Patricio Gonzalez Vivo on 3/11/13.
//
//
//  Using Arduino as Server and your laptop (tested on OSX) as a client:
//
//      1. Flash your Arduino with this firmware: http://pulse-sensor.googlecode.com/files/PulseSensorAmped_Arduino_1dot1.zip
//
//      2. Connect the Arduino by USB to this computer
//
//      3. Add this addon to your program (see arduino-example) and compile
//
//  Raspberry Pi:
//
//      1. RaspberryPi don't have analog-in like Arduino. That's why we are going to use [**MCP3008**](https://www.adafruit.com/products/856) to transform the resistance of the photocell into pulse frequency. Maybe you also want to by the [Pi Cobbler breakout](http://www.adafruit.com/products/914) to make the connections easier.
//
//      2. Everything is almost like [this tutorial](http://learn.adafruit.com/send-raspberry-pi-data-to-cosm) except that we are not using ```#23```, ```#24``` and ```#25``` instead we are using SPI just like [Jason Van Cleave](https://github.com/jvcleave) and [Andreas Muller](https://github.com/andreasmuller) did in [this example](https://github.com/andreasmuller/RaspberryPiWorkshop/tree/master/wiringPiPotentiometerExample). Off course instead of using a potentiometer the PulseSensor.
//
//          [photo](https://raw.github.com/andreasmuller/RaspberryPiWorkshop/master/wiringPiPotentiometerExample/wiringPiPotentiometerExampleSPI_bb.jpg)
//
//      3. Configure your RaspberryPi for wiringPi ( https://github.com/openFrameworks-RaspberryPi/openFrameworks/wiki/Raspberry-Pi-Using-the-GPIO-pins-with-Wiring-Pi-and-openFrameworks ).
//
//          a. ```sudo nano /etc/modprobe.d/raspi-blacklist.conf```
//          b. comment with # (or remove) those lines
//          c. reboot the pi
//
//  More References:
//
//      - http://learn.adafruit.com/reading-a-analog-in-and-controlling-audio-volume-with-the-raspberry-pi/connecting-the-cobbler-to-a-mcp3008
//      - http://raspberrypihobbyist.blogspot.com/2012/12/analog-interface.html
//
#pragma once

#include "ofMain.h"

#ifdef TARGET_RASPBERRY_PI
#include "wiringPiSPI.h"
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
