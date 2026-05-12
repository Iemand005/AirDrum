#include <Arduino.h>
#include <RCSwitch.h>


#define pinReceiver 2 // GPIO pin connected to the data pin of the 433MHz receiver module. ESP32 supports interrupts on almost any GPIO, so you can choose a different pin if needed.
#define pinLed 3

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
}

void loop() {

    digitalWrite(pinLed, ledStatus);

    if (mySwitch.available()) {
        unsigned long receivedCode = mySwitch.getReceivedValue();

        if (receivedCode == 0) {
            Serial.println("Unknown encoding");
        } else {
          ledStatus = !ledStatus;  // Toggle LED status on each received code
            Serial.print("Received code: ");
            Serial.print(receivedCode);
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