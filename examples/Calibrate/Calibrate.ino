// Calibrate the servo for good angle accuracy, manually or automatically.

#include <ESP32Servo360.h>

ESP32Servo360 servo;

void setup() {
  servo.attach(4, 16); // Control pin (white), signal pin (yellow).
  servo.adjustSignal(2, 1000); // Setting manually the wrong PWMs, defaults are 32 & 1067, min then max.
  Serial.begin(9600); // Open the console to see the result of the calibration.
  servo.calibrate(); // Setting accurate PWMs by comparing while spinning slowly.
}

void loop() {
  
}
