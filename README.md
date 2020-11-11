
# ESP32Servo360.h
## Version 0.1.0

Control Parallax High Speed 360° servos with ESP32 boards. Works with interrupts and multitasking (FreeRTOS).
About this motor:
- [Infos](https://www.parallax.com/product/900-00360).
- [Product Guide](https://www.parallax.com/sites/default/files/downloads/900-00360-Feedback-360-HS-Servo-v1.2.pdf) [PDF].

## How to

#### Suggested equipment
- 6V 2A power supply.
- ESP32 Dev board.
- Breadboard.
- Jumper wires.
- Parallax High Speed 360° servos
- Micro USB to USB 2.0

#### Installation for ESP32 boards
1. Download the latest release [here](https://github.com/ecal-mid/ESP32Servo360/releases). Then, _Assets > Source code (zip)_.
2. Download the latest [Arduino IDE](https://www.arduino.cc/en/software) then open it. 
3. Go to _File > Preferences_, add `https://dl.espressif.com/dl/package_esp32_index.json` to *Additional Boards Manager URLs*, don't forget to separate the URLs with a comma.
4. In _Board > Boards Manager_, search and install the latest core _esp32_.

#### Connecting the ESP32 to a Servo
1. Wire like this image below. Make sure to connect the ESP32 GND pin to the ground of the 6V power supply.
![Wiring](https://raw.githubusercontent.com/ecal-mid/ESP32Servo360/main/docs/wiring.jpg)
3. Connect your esp32 to your computer and the motor to your 6V power supply.
4. On Arduino IDE, find your board in _Tools > Port_. Mine, shows as _/dev/cu.SLAB_USBtoUART_.
5. In _Board > Boards Manager > ESP32 Arduino_, select _"DOIT ESP32 DEVKIT V1"_.
6. Run one of the examples (_File > Examples > ESP32Servo360_). Check if the pins matches the code `servo.attach(4, 16)`.

## Instantiation
#### ESP32Servo360 servo;
Or
#### ESP32Servo360 servo(```[int  offsetAngle]```, ```[int rpm]```, ```[int deceleration]```, ```[int minPulseWidth]```, ```[int maxPulseWidth]```, ```[int channel]```);
Set optionnal parameters directly at the creation of an instance.
- ```offsetAngle``` Default is 0 degrees.
- ```rpm``` Default speed of the servo (Turns  per  Minute). Maximum is 140.
- ```deceleration``` Ease-out. By default, deceleration starts at 70 degrees from target angle.
- ```minPulseWidth``` PWM signal of the Hall Sensor at 0 degrees. Default is 32.
- ```maxPulseWidth``` PWM signal of the Hall Sensor at ~360 degrees. Default is 1067.
- ```channel``` ESP32 LEDC channel used to send the PWM signal to the motor.

## Methods
### Settings
#### servo.adjustSignal(```int minPulseWidth```, ```int maxPulseWidth```)
For better readable angle accuracy, manually set the PWM signal of the internal Hall Effect Sensor.
- Default ```minPulseWidth``` is 32. Signal at 0 degrees.
- Default ```maxPulseWidth``` is 1067. Signal at ~360 degrees.
- Get the values with the method ```servo.calibrate();```.
#### servo.attach(`int controlPin`, `int feedbackPin`)
Attach to the correct pins of your servo.
- ```controlPin``` is the white cable.
- ```feebackPin``` is the yellow cable.
#### servo.calibrate()
For better readable angle accuracy, automatically set the PWM signal of the internal Hall Effect Sensor.\
The servo will turn and give you the result through `Serial`.
#### servo.detach()
Disconnect the servo from its pins.
#### servo.setDeceleration(```int deceleration```)
To prevent any bouncing, especially due to high RPM.\
You can start the deceleration to a higher angle from the target angle.\
Default is set to _180_ degrees.
#### servo.setOffset(```int offsetAngle```)
If you decide to change the orientation of your motor, you can offset its zero position.
#### servo.setSpeed(```float rpm```)
Change the maximum RPM (Rotation Per Minute) of your servo.
Default is 70 RPM.
### Actions
#### servo.clearTurns()
Reset the number of turns.
#### servo.hold()
Stop and hold the servo to its current angle. Will be harder to turn the motor by hand.
Disable this state by executing another rotation or calling ```servo.release();```.
#### servo.release()
Releases the servo from its hold state.
#### servo.rotate(```float angle```)
Rotate from current position.
#### servo.rotateTo(```float target```)
Rotate from to a specific position.
#### servo.spin(```[float rpm]```)
Spin clockwise or anticlockwise at a the default RPM if the parameter unset.\
Setting the parameter won't change the saved RPM of the servo. Instead use the method ```servo.setSpeed();``` .
Value must be between -140 and 140.
#### servo.stop()
Stop the rotation of the servo.
#### servo.wait()
Wait for the motor to finish its rotation. Will hold the execution of the main loop().
### States
#### servo.attached()
```true``` if servo is attached to pins.
Returns bool.
#### servo.busy()
```true``` if servo is still executing a rotation or holding an angle.\
Returns bool.
### Feedback
#### servo.getAngle()
Get the amount of rotation. Angle will go below 0 and above 360 degrees.\
Returns a float.
#### servo.getOrientation()
Get the orientation between 0 to 360 degrees.\
Returns a float.
#### servo.getTurns()
Get number of turns. This will be reset after a reboot of the board.\
Returns int.

----
Author, maintainer: [Sébastien Matos](https://github.com/matoseb)\
**ECAL 2020 Bachelor Media & Interaction design.**
![ECAL](https://www.ecal.ch/img/logo.png)
