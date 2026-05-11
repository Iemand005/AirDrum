# AirDrum

This repo contains the C++ source code for Arduino Nano and ESP32

Arduino Nano is long range Whadda server. (WPI469T)

ESP32 is Whadda receiver. (WPI469R)


In theory we can also get gyroscope and magnetometer data. 
Magnetometer could be used to measure strength of a magnetic field, so we could make it make noises if it's moved near a field. 


## Wire arrangement

### Arduino

  All the GND to GND

  Arduino 3V3 -> + rail
  3.3V + rail -> WPI469T VCC
  3.3V + rail -> Accelerometer VCC
  
For new just read the code to see what pins to connect to which components we should probably make al ittle board schematic to wire it right



### ESP32

