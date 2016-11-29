#include "LightSensor.h"

#define LIGHT_DIFF_THRESHOLD 100

LightSensor::LightSensor(int pin) {
    this->pin = pin;
    this->dark = false;
    this->lastValue = analogRead(pin);
    this->nextTimestamp = millis();
}

bool LightSensor::read() {
    bool ret = false;
    unsigned long now = millis();

    if (now >= nextTimestamp) {
        int val = analogRead(pin);
        int diff = val - lastValue;

        if (dark) {
            if (diff > LIGHT_DIFF_THRESHOLD) {
                // change of state -> light
                ret = true;
                dark = false;
            }
        }
        else {
            // detecting dark
            if (diff < -LIGHT_DIFF_THRESHOLD) {
                // change of state -> dark
                ret = true;
                dark = true;
            }
        }

        lastValue = val;
        nextTimestamp = now + 100;
    }

    return ret;
}
