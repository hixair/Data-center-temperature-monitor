WIP (2019-04-08)

# Data-center-temperature-monitor
A temperature monitor based on and ESP32 and DS18B20 sensors, that can be mounted in a server rack.


![Temperaturemonitor](img/IMG_5406.JPG "temperature monitor")

## The project

I know, yet another temperature monitor ...
This is my first complete project that involves CAD design, 3D printing, electronics prototyping, C code (I am not a developer ;) and webhooks.

Goals of this project : 
- Getting temperatures from the 3 sensors (AC, back of the cabinets and ambient) 
- Log the temperatures and graph these to monitor trends overtime.
- Send alerts when a level is reached.

## The controller

I initially prototyped the temperature monitor using the typical breadboard :

![breadboard](img/IMG_2920.JPG "breadboard")

## Connecting the sensors

I used mini-XLR connectors as they hav 3 pins and a locking lever. 
The DS18B20 plug on a one wire bus, it means that they are all connected to the same wire and you can multiply "branches" of you tree as much as you need (as long as you power these accordingly).

![The Wire](img/The_Wire.jpg "TheWire")

I used a old epson ribbon printer cartridge to group 3 female plugs at the end of the main wire. I could go with another group of sensors coming from one of these plugs.

## The Case

The first interation of the case revealed that wifi does not do well when the antenna of the controller is situated between 2 server blades (and the access to the usb plug was not ideal anyway) :

![first case](img/IMG_1958.JPG "first case")

I messed up some dimentions and had to go back to f360 to fix the problems :

![3D printing is awesome](img/IMG_0819.JPG "3D printing is awesome")

Final case :

![Cold Case](img/Cold_Case.jpg "ColdCasee")



