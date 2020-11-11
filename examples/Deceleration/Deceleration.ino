// Control the deceleration to smoothen the rotation

#include <ESP32Servo360.h>

ESP32Servo360 servo;

void setup() {
  servo.attach(4, 16); // Control pin (white), signal pin (yellow).
  servo.setSpeed(140);
  servo.rotateTo(0);
  servo.wait();
}

void loop() {
  servo.setDeceleration(45); // Starts decelerating at 45 degrees from target angle. It will overshoot and bounce if spinning too fast.
  servo.rotateTo(360);
  servo.wait();
  servo.setDeceleration(360); // Default deceleration is 180 degrees.
  servo.rotateTo(-360);
  servo.wait();
}
