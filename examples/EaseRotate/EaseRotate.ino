// Set angle and wait for the given amount of time.

#include <ESP32Servo360.h>

ESP32Servo360 servo;

void setup() {
  servo.attach(4, 16); // Control pin (white), signal pin (yellow).
  servo.setSpeed(140);
  servo.rotateTo(0); // Reset position
  servo.wait(); // Wait end of movement
  servo.easeRotateTo(360); // rotate complete turn clockwise
  servo.wait();
  servo.setSpeed(40);
}

void loop() {
  servo.easeRotate(60); // ease in out
  servo.wait();
  servo.easeRotate(60);
  servo.wait();
  servo.easeRotate(-30);
  servo.wait();
}
