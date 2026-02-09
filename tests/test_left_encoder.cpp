#include <Arduino.h>
#include "pins.h"

void setup() {
    Serial.begin(9600);
    delay(1000);
    
    pinMode(ENCODER_LEFT_A, INPUT_PULLUP);
    pinMode(ENCODER_LEFT_B, INPUT_PULLUP);
    
    Serial.println("Spin LEFT wheel SLOWLY by hand...");
    Serial.println("Watching pins 32 and 33:\n");
}

void loop() {
    Serial.print("Left A (pin 32): ");
    //delay(1000);
    Serial.print(digitalRead(ENCODER_LEFT_A));
    //delay(1000);
    Serial.print(" | Left B (pin 33): ");
    //delay(1000);
    Serial.println(digitalRead(ENCODER_LEFT_B));
    delay(2000);
}