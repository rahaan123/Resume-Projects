#include <Arduino.h>

#define FAN_PIN 12 // Pin connected to the fan relay

void setup() {
  pinMode(FAN_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      digitalWrite(FAN_PIN, HIGH);  // Turn fan on
    } else if (command == '0') {
      digitalWrite(FAN_PIN, LOW);   // Turn fan off
    }
  }
}

