#include <Arduino.h>
#include <RCSwitch.h>

#include <Adafruit_SH1106.h>
#include <Wire.h>

#define i2C_ADDRESS 0x3C

#define pinReceiver 2 // GPIO pin connected to the data pin of the 433MHz receiver module. ESP32 supports interrupts on almost any GPIO, so you can choose a different pin if needed.
#define pinLed 3

Adafruit_SH1106 display;

// Radio stuff
RCSwitch mySwitch = RCSwitch();

int code = 0;
bool ledStatus = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting receiver...");

    pinMode(pinLed, OUTPUT);

    // mySwitch.enableReceive(pinReceiver);

    mySwitch.enableReceive(digitalPinToInterrupt(pinReceiver));

    Serial.println("Setup complete");

    display.begin(SH1106_SWITCHCAPVCC, i2C_ADDRESS);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Meow");
    display.display();
}

void loop() {

    digitalWrite(pinLed, ledStatus);

    if (mySwitch.available()) {
        unsigned long receivedCode = mySwitch.getReceivedValue();

        long code = receivedCode / 1000;
        int value = receivedCode % 1000;

        if (receivedCode == 0) {
            Serial.println("Unknown encoding");
        } else {
          ledStatus = !ledStatus;  // Toggle LED status on each received code
            Serial.print("Received code: ");
            Serial.print(code);
            Serial.print("Value: ");
            Serial.print(value);
            Serial.print(" / bitlength: ");
            Serial.print(mySwitch.getReceivedBitlength());
            Serial.print(" / delay: ");
            Serial.print(mySwitch.getReceivedDelay());
            Serial.print(" / protocol: ");
            Serial.println(mySwitch.getReceivedProtocol());
        }

        mySwitch.resetAvailable();
    }
}