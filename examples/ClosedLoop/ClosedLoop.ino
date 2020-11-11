// Hold at given angle. Behaves like a closed loop circuit.

#include <ESP32Servo360.h>

ESP32Servo360 servo;

void setup() {
  servo.attach(4, 16); // Control pin (white), signal pin (yellow).
  servo.setSpeed(140);
  servo.rotateTo(0);
  servo.wait();
}

void loop() {

  servo.rotateTo(0);
  servo.wait(); // Turn the motor by hand.
  delay(3000);
  
  servo.rotateTo(360); // Starts decelerating at 45 degrees from target angle. It will overshoot and bounce if spinning too fast.
  servo.wait();
  servo.hold(); // Servo is holding to last angle. Rotating by hand is difficult now.
  delay(3000);
  servo.release(); // Use this function to release, holding state will be removed after any rotation as well.
}
