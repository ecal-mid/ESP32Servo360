// Set angle and wait for the given amount of time.

#include <ESP32Servo360.h>

ESP32Servo360 servo;

void setup() {
  servo.attach(4, 16); // Control pin (white), signal pin (yellow).
  servo.setSpeed(140);
  servo.rotateTo(720); // Set absolute angle to 720 degrees.
}

void loop() {
  servo.wait(); // Wait for end of previous rotation.
  delay(1000);
  servo.rotate(90); // Rotate by 90 degrees.
  servo.wait();
  servo.rotate(-45);
  servo.wait();
  servo.rotate(180);
}
