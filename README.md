# AirDrum

This repo contains the C++ source code for Arduino Nano and ESP32

Arduino Nano is long range Whadda server. (WPI469T)

ESP32 is Whadda receiver. (WPI469R)


In theory we can also get gyroscope and magnetometer data. 
Magnetometer could be used to measure strength of a magnetic field, so we could make it make different noises if it's moved near a magnetic field (like one of a speaker or something).
Gyroscope can  be used to make instrument controls where you can change volume or pitch by rotating the device in any axis


## Wire arrangement

### Arduino

  All the GND to GND

  Arduino 3V3 -> + rail
  3.3V + rail -> WPI469T VCC
  3.3V + rail -> Accelerometer VCC
  
For new just read the code to see what pins to connect to which components we should probably make al ittle board schematic to wire it right



### ESP32

