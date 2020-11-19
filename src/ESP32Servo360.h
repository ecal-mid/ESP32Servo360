/*
 * Parallax 360 Servo Motor
 *
 * Sébastien Matos
 * 
 * 2020 ECAL
 */
#ifndef ESP32_SERVO_360
#define ESP32_SERVO_360
#include <Arduino.h>

#if defined(ESP32)
#include <WiFiClientSecure.h>
#else
#error “This library only supports ESP32 boards for now.”
#endif

// #define FILLARRAY(a,n) a[0]=n, memcpy( ((char*)a)+sizeof(a[0]), a, sizeof(a)-sizeof(a[0]) );

class ESP32Servo360
{
    static const int ZERO_ANGLE = 0;
    static const int RPM = 70;
    static const int DECELERATION = 180;     //start deceleration at a delta angle of
    static const int MIN_PULSE_WIDTH = 32;   // the shortest pulse sent by a servo 360
    static const int MAX_PULSE_WIDTH = 1067; // the longest pulse sent by a servo 360
    static const int CHANNEL_MAX_NUM = 16;

    static const int MAX_RPM = 140;
    static const int MIN_RPM = 5; //ease minimal speed = MIN_RPM + MIN_TORQUE
    static const int MIN_TORQUE = 7;

public:
    static const int CHANNEL_NOT_ATTACHED = -1;
    static const int PIN_NOT_ATTACHED = -1;

    ESP32Servo360(int offsetAngle = ZERO_ANGLE, int rpm = RPM, int deceleration = DECELERATION, int minPulseWidth = MIN_PULSE_WIDTH, int maxPulseWidth = MAX_PULSE_WIDTH, int channel = CHANNEL_NOT_ATTACHED);
    ~ESP32Servo360();
    bool attach(int ctrlPin, int feedbackPin);
    bool detach();
    void wait();
    void stop();
    void hold();
    void calibrate();
    void release();
    void spin(float rpm);
    void spin(void);
    bool busy();
    bool attached() const;
    void adjustSignal(int minPulseWidth, int maxPulseWidth);
    void setOffset(int offsetAngle);
    void setDeceleration(int deceleration);
    void rotate(float angle);
    void rotateTo(float target);
    void easeRotate(float target);
    void easeRotateTo(float target);
    void setMinSpeed(float minRpm);
    void setSpeed(float maxRpm);
    void setMinimalForce(float minTorque);
    void setAdditionalTorque(float minRpm);
    int getSpeed();
    void clearTurns();
    float getAngle();
    float getOrientation();
    int getTurns();

private:
    void IRAM_ATTR _rising();
    void IRAM_ATTR _falling();
    void _beginLoop();
    void _beginHold();
    void _beginEase();
    void _computeAngle();
    void _computeTarget();
    void _resetFields();
    inline void _disableRunningTask();
    void _setRPM(float rpm);

    //utils
    static float _fmap(float x, float in_min, float in_max, float out_min, float out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    static int8_t _sgn(int val)
    {
        if (val < 0)
            return -1;
        if (val == 0)
            return 0;
        return 1;
    }

    static float easeInOutQuad(float t)
    {
        return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
    }

    static void IRAM_ATTR _isr(void *arg)
    {

        ESP32Servo360 *s = static_cast<ESP32Servo360 *>(arg);

        if (digitalRead(s->_feedbackPin))
        { //FALLING
            s->_prevTime = esp_timer_get_time();
        }
        else
        {
            s->_pwmValue = esp_timer_get_time() - s->_prevTime;
        }
    }

    static void _updateHold(void *arg)
    {
        ESP32Servo360 *s = static_cast<ESP32Servo360 *>(arg);
        for (;;)
        { // infinite loop
            s->_computeAngle();
            s->_computeTarget();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }

    static void _updateLoop(void *arg)
    {
        ESP32Servo360 *s = static_cast<ESP32Servo360 *>(arg);

        byte len = 32;
        int samples[len];
        int n = 100;
        byte slot = 0;
        int total = n * len;

        for (byte i = 0; i < len; i++)
        {
            samples[i] = n;
        }

        for (;;)
        { // infinite loop

            s->_computeAngle();
            s->_computeTarget();
            int speed = round(s->_speed);

            total = total - samples[slot] + speed;
            samples[slot] = speed;
            slot = (slot + 1) % len;

            if (abs((float)total / len - speed) < 1 && abs(speed) < 6)
            {
                s->_updateHandle = NULL;
                vTaskDelete(NULL);
            }

            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }

    static void _updateEase(void *arg)
    {
        ESP32Servo360 *s = static_cast<ESP32Servo360 *>(arg);

        s->_computeAngle();
        float origAngle = s->_angle; // Get complete angle.
        float currAngle = origAngle;
        float peak = 0.5;
        float t = 0;
        float minForce = min(s->_minTorque + s->_minRpm, (float)s->_rpm);


        int direction = _sgn(s->_target - origAngle);

        while (t < 1.0)
        {
            Serial.println(t);
            s->_computeAngle();
            float currAngle = s->_angle;
            t = _fmap(currAngle, origAngle, s->_target, 0, 1);

            float e = (easeInOutQuad(t) - peak);
            float speed = _fmap(abs(e), 0, peak, s->_rpm, minForce);
            s->_setRPM(speed * direction);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);

        s->_setRPM(0);
        s->_updateHandle = NULL;
        vTaskDelete(NULL);
    }

    static int channel_next_free;

    int _ctrlPin;
    int _feedbackPin;
    int _channel;
    int _minPulseWidth, _maxPulseWidth;

    volatile uint16_t _pwmValue;
    volatile int64_t _prevTime;

    float _orientation;
    float _angle;
    float _speed;
    int _rpm;
    float _minTorque;
    float _minRpm;
    int _offsetAngle;
    int _target;
    bool _hold;
    int _deceleration;
    TaskHandle_t _updateHandle;
};

#endif