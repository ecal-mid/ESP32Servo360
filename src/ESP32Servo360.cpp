#include <ESP32Servo360.h>

int ESP32Servo360::channel_next_free = 0;

ESP32Servo360::ESP32Servo360(int offsetAngle, int rpm, int deceleration, int minPulseWidth, int maxPulseWidth, int channel)
{
    _resetFields();
    _offsetAngle = offsetAngle;
    _target = offsetAngle;
    _rpm = rpm;
    _minTorque = MIN_TORQUE;
    _minRpm = MIN_RPM;
    _minPulseWidth = minPulseWidth;
    _maxPulseWidth = maxPulseWidth;
    _updateHandle = NULL;
    _speed = 0;

    _hold = false;
    _pwmValue = -1; // unset
    _deceleration = deceleration;
};

ESP32Servo360::~ESP32Servo360()
{
    detach();
}

bool ESP32Servo360::attach(int ctrlPin, int feedbackPin)
{

    if (_channel == CHANNEL_NOT_ATTACHED)
    {
        if (channel_next_free == CHANNEL_MAX_NUM)
        {
            return false;
        }
        else
        {
            _channel = channel_next_free;
            channel_next_free++;
        }
    }

    _ctrlPin = ctrlPin;
    _feedbackPin = feedbackPin;

    ledcSetup(_channel, 50, 16); // channel X, 50 Hz, 16-bit depth
    ledcAttachPin(_ctrlPin, _channel);

    pinMode(_feedbackPin, INPUT);
    attachInterruptArg(_feedbackPin, _isr, this, CHANGE);

    // _beginLoop();
    delay(100); // wait for first angle calculation

    _computeAngle();

    if (_angle < 0)
    {
        _angle += 360;
        _orientation = _angle;
    }
    Serial.println(_angle);

    return true;
}

void ESP32Servo360::setOffset(int offsetAngle)
{
    float trueTarget = _target - _offsetAngle;
    _offsetAngle = offsetAngle;
    _target = trueTarget + _offsetAngle;
}

void ESP32Servo360::setDeceleration(int deceleration)
{
    _deceleration = deceleration;
}

void ESP32Servo360::adjustSignal(int minPulseWidth, int maxPulseWidth)
{
    _maxPulseWidth = constrain(maxPulseWidth, 0, 1500);
    _minPulseWidth = constrain(minPulseWidth, 0, maxPulseWidth);
}

void ESP32Servo360::calibrate()
{
    _disableRunningTask();
    _setRPM(12);
    delay(20);
    _computeAngle();
    float origAngle = _angle;
    float minPulseWidth = 1500;
    float maxPulseWidth = 0;

    while (origAngle + 720 > _angle)
    {
        Serial.println(origAngle);
        delay(1);
        _computeAngle();

        if (_pwmValue < minPulseWidth && _pwmValue >= 0)
            minPulseWidth = _pwmValue;
        if (_pwmValue > maxPulseWidth && _pwmValue < 1500)
            maxPulseWidth = _pwmValue;
    }
    rotateTo(origAngle);
    wait();
    adjustSignal(minPulseWidth, maxPulseWidth);
    Serial.print("Minimal PWM: ");
    Serial.print(_minPulseWidth);
    Serial.print(", maximal PWM: ");
    Serial.println(_maxPulseWidth);
}

void ESP32Servo360::spin(float rpm)
{
    _disableRunningTask();
    _setRPM(rpm);
}

void ESP32Servo360::spin(void)
{
    _disableRunningTask();
    _setRPM(_rpm);
}

void ESP32Servo360::wait()
{
    while (_updateHandle != NULL) {
        delay(1);
    } 
}

bool ESP32Servo360::busy()
{
    return _updateHandle = NULL;
}

void ESP32Servo360::setSpeed(float maxRpm)
{
    _rpm = constrain(maxRpm, -MAX_RPM, MAX_RPM);
    _minRpm = constrain(_minRpm, 0, _rpm);
}

void ESP32Servo360::setAdditionalTorque(float minRpm) {
    _minRpm = constrain(minRpm, 0, _rpm);
}

void ESP32Servo360::setMinimalForce(float minTorque)
{
    _minTorque = max(minTorque, (float)0);
}

void ESP32Servo360::rotate(float angle)
{
    _target += angle;
    _beginLoop();
}

void ESP32Servo360::rotateTo(float target)
{
    _target = target + _offsetAngle;
    _beginLoop();
}
void ESP32Servo360::easeRotateTo(float target)
{
    _target = target + _offsetAngle;
    _beginEase();
}

void ESP32Servo360::easeRotate(float angle)
{
    _target += angle;
    _beginEase();
}

bool ESP32Servo360::detach()
{
    if (!this->attached())
    {
        return false;
    }

    if (_channel == (channel_next_free - 1))
        channel_next_free--;

    ledcDetachPin(_ctrlPin);
    detachInterrupt(_feedbackPin);
    _resetFields();
    return true;
}

float ESP32Servo360::getAngle()
{
    _computeAngle();
    return _angle - _offsetAngle;
}

int ESP32Servo360::getTurns()
{
    float angle = getAngle();
    return (angle - fmod(angle, 360)) / 360;
}

float ESP32Servo360::getOrientation()
{
    float angle = getAngle();
    return fmod(angle, 360);
}

void ESP32Servo360::clearTurns()
{
    _angle = _orientation;
}

int ESP32Servo360::getSpeed()
{
    return _rpm;
}

void ESP32Servo360::hold()
{

    if (!_hold)
    {
        _disableRunningTask();
        _hold = true;
        delay(100);
        _computeAngle();
        _target = _angle;
        _beginHold();
    }
}
void ESP32Servo360::release()
{
    _hold = false;
    stop();
}

bool ESP32Servo360::attached() const { return _ctrlPin != PIN_NOT_ATTACHED; }

void ESP32Servo360::stop()
{
    _disableRunningTask();
}

void ESP32Servo360::_computeAngle()
{
    float newOrientation = _fmap(_pwmValue, _minPulseWidth, _maxPulseWidth, 0, 360);
    float delta = newOrientation - _orientation;

    if (delta > 180)
    {
        _angle -= 360;
    }
    else if (delta < -180)
    {
        _angle += 360;
    }

    _angle += delta;
    _orientation = newOrientation;
}

void ESP32Servo360::_computeTarget()
{
    float theta = _target - _angle;
    _speed = constrain(_fmap(abs(theta), 0, _deceleration, _minTorque, MAX_RPM), 0, _rpm) * _sgn(theta);
    _setRPM(_speed);
}

inline void ESP32Servo360::_disableRunningTask()
{
    if (_updateHandle != NULL)
    {
        vTaskDelete(_updateHandle);
        _updateHandle = NULL;
    }

    _setRPM(0);
}

void ESP32Servo360::_setRPM(float rpm)
{

    //https://www.parallax.com/sites/default/files/downloads/900-00360-Feedback-360-HS-Servo-v1.2.pdf

    float tControl = 1500; //Default 0 RPM
    rpm = constrain(rpm, -MAX_RPM, MAX_RPM);

    if (rpm < 0)
    {
        tControl = _fmap(rpm, -MAX_RPM, 0, 1720, 1520);
    }
    else if (rpm > 0)
    {
        tControl = _fmap(rpm, 0, MAX_RPM, 1480, 1280);
    }

    float duty = tControl / (20000 /*20ms*/ / 65536.0 /*2 power of 16(bit)*/);

    ledcWrite(_channel, duty);
}

void ESP32Servo360::_resetFields(void)
{
    _feedbackPin = PIN_NOT_ATTACHED;
    _ctrlPin = PIN_NOT_ATTACHED;
    _channel = CHANNEL_NOT_ATTACHED;
    _offsetAngle = ZERO_ANGLE;
    _minPulseWidth = MIN_PULSE_WIDTH;
    _maxPulseWidth = MAX_PULSE_WIDTH;
}

void ESP32Servo360::_beginLoop()
{
    _disableRunningTask();
    _hold = false;
    xTaskCreate(
        _updateLoop,
        "update",
        1000,
        this,
        1,
        &_updateHandle // Task handle
    );
}

void ESP32Servo360::_beginEase()
{
    _disableRunningTask();
    _hold = false;
    xTaskCreate(
        _updateEase,
        "ease",
        1000,
        this,
        1,
        &_updateHandle // Task handle
    );
}

void ESP32Servo360::_beginHold()
{
    _disableRunningTask();

    xTaskCreate(
        _updateHold,
        "hold",
        1000,
        this,
        1,
        &_updateHandle // Task handle
    );
}