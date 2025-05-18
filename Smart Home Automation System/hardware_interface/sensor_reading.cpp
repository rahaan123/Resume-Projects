#include <Arduino.h>

#define TEMP_SENSOR_PIN A0 // Analog pin connected to temperature sensor

void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);  // Convert to voltage
  float temperature = (voltage - 0.5) * 100; // Convert voltage to temperature (Celsius)

  Serial.print("Temperature: ");
  Serial.println(temperature);
  delay(2000);  // Delay between readings
}

