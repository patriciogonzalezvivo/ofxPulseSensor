#ofxPulseSensor
######by [Patricio Gonzalez Vivo](http://patriciogonzalezvivo.com)

![pulse](http://cdn.shopify.com/s/files/1/0100/6632/products/PulseSensorAmpedFinger-web_2_large.jpg?14)

OpenFramework addon to read that from [PulseSensor](http://pulsesensor.myshopify.com/) develop by Joel Murphy and Yury Gitman.

### Connecting PulseSensor through Arduino
Using Arduino as Server and your laptop (tested on OSX) as a client:

1. Flash your Arduino with this [firmware](http://pulse-sensor.googlecode.com/files/PulseSensorAmped_Arduino_1dot1.zip)
2. Connect the Arduino by USB to this computer
3. Add this addon to your program (see arduino-example) and compile

### Connecting your PulseSensor directly OpenFrameworks App using Raspberry Pi:

![photo](https://raw.github.com/andreasmuller/RaspberryPiWorkshop/master/wiringPiPotentiometerExample/wiringPiPotentiometerExampleSPI_bb.jpg)

1. RaspberryPi don't have analog-in like Arduino. That's why we are going to use [**MCP3008**](https://www.adafruit.com/products/856) to transform the resistance of the photocell into pulse frequency. Maybe you also want to by the [Pi Cobbler breakout](http://www.adafruit.com/products/914) to make the connections easier.

2. Everything is almost like [this tutorial](http://learn.adafruit.com/send-raspberry-pi-data-to-cosm) except that we are not using ```#23```, ```#24``` and ```#25``` instead we are using SPI just like [Jason Van Cleave](https://github.com/jvcleave) and [Andreas Muller](https://github.com/andreasmuller) did in [this example](https://github.com/andreasmuller/RaspberryPiWorkshop/tree/master/wiringPiPotentiometerExample). Off course instead of using a potentiometer the PulseSensor.

3. Configure your RaspberryPi for wiringPi ( https://github.com/openFrameworks-RaspberryPi/openFrameworks/wiki/Raspberry-Pi-Using-the-GPIO-pins-with-Wiring-Pi-and-openFrameworks ).

	1. ```sudo nano /etc/modprobe.d/raspi-blacklist.conf```
	2.	comment with # (or remove) those lines
	3.	reboot the pi

More References about Analog Input in RaspberryPi:

* http://learn.adafruit.com/reading-a-analog-in-and-controlling-audio-volume-with-the-raspberry-pi/connecting-the-cobbler-to-a-mcp3008
* http://raspberrypihobbyist.blogspot.com/2012/12/analog-interface.html

## Original PulseSensor Documentation

* [Guide](http://pulse-sensor.googlecode.com/files/PulseSensorAmpedGettingStartedGuide.pdf)
* [Web](http://pulsesensor.myshopify.com/)
* [Hardware Information](http://pulsesensor.myshopify.com/pages/open-hardware)
* [Video](https://vimeo.com/58657081)