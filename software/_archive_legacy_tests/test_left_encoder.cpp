#include <Arduino.h>
#include "pins.h"

void setup() {
    Serial.begin(9600);
    delay(1000);
    
    pinMode(32, INPUT);
    pinMode(33, INPUT);
    
    Serial.println("Spin LEFT wheel SLOWLY by hand...");
    Serial.println("Watching pins 32 and 33:\n");
}

void loop() {
    Serial.print("Left A (pin 32): ");
    //delay(1000);
    Serial.print(digitalRead(32));
    //delay(1000);
    Serial.print(" | Left B (pin 33): ");
    //delay(1000);
    Serial.println(digitalRead(33));
    delay(2000);
}