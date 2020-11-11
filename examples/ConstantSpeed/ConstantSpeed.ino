#include <ESP32Servo360.h>

ESP32Servo360 servo;

void setup() {
  servo.attach(4, 16); // Control pin (white), signal pin (yellow).
  servo.setSpeed(140); // Set turns per minute (RPM), 140 max.
}

void loop() {
    servo.spin(); // Turn at set speed clockwise.
    delay(2000);
    servo.spin(-40); // Turn at 40 RPM anticlockwise.
    delay(2000);
}
