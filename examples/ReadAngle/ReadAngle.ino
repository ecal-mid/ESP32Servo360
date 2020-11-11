// Read angle, turns, orientations.

#include <ESP32Servo360.h>

ESP32Servo360 servo;

void setup() {
  servo.attach(4, 16); // Control pin (white), signal pin (yellow).
  Serial.begin(9600);
}

void loop() {
  float angle, turns;

  servo.rotateTo(720);
  servo.wait();
  angle = servo.getAngle(); // Get complete angle.
  Serial.println(angle);

  servo.rotateTo(-475);
  servo.wait();
  angle = servo.getOrientation(); // 0 to 360 degrees.
  turns = servo.getTurns(); // Number of full turns.
  Serial.println((String)angle + "°, " + turns + "x."); // Turns won't be saved after a reboot.
}
