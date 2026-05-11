#include <Arduino.h>
#include <RCSwitch.h>

#define pinButton 5
#define pinLed 4
#define pinTransmitter 2

RCSwitch mySwitch = RCSwitch();

int code = 0;
bool isButtonPressed = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting server...");

    pinMode(pinButton, INPUT_PULLUP);
    pinMode(pinLed, OUTPUT);

    mySwitch.enableTransmit(pinTransmitter);

    Serial.println("Setup complete");
}

void loop() {
    bool digitalReadButton = !digitalRead(pinButton);

    if (digitalReadButton == HIGH && !isButtonPressed) {
        isButtonPressed = true;

        Serial.println("Button pressed");
        digitalWrite(pinLed, HIGH);

        code++;
        mySwitch.send(code, 24);
        
    } else if (digitalReadButton == LOW && isButtonPressed) {
        isButtonPressed = false;

        Serial.println("Button released");
        digitalWrite(pinLed, LOW);
    }

    delay(100);
}