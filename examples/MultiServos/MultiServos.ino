// Connect up to theoretically 16 servos. (Max number off ledc Channels on the ESP32).

#include <ESP32Servo360.h>

ESP32Servo360 servo1, servo2;

void setup() {
  servo1.attach(4, 16); // Control pin (white), signal pin (yellow).
  servo2.attach(25, 26);

  servo1.setSpeed(140);
  servo2.setSpeed(140);
}

void loop() {
  servo1.rotateTo(0);
  servo2.rotateTo(0);
  servo1.wait();
  servo2.wait();

  servo1.rotateTo(-675);
  servo2.spin(50);
  servo1.wait();
  servo2.stop();

  delay(1000);
  
  servo1.rotate(90);
  delay(1000);
  servo2.rotateTo(360);

  servo1.wait();
  servo2.wait();
}
