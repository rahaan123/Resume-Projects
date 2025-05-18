#include <Arduino.h>

#define LIGHT_PIN 13 // Pin connected to the light relay

void setup() {
  pinMode(LIGHT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      digitalWrite(LIGHT_PIN, HIGH);  // Turn light on
    } else if (command == '0') {
      digitalWrite(LIGHT_PIN, LOW);   // Turn light off
    }
  }
}
