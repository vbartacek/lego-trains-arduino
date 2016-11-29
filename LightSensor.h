
#ifndef LightSensor_h
#define LightSensor_h
#include "Arduino.h"

class LightSensor {

    public:
        /**
         * Creates a new instance.
         * @param pin the input pin number
         */
        LightSensor(int pin);

        /**
         * Reads the input pin and detects change of the state.
         * @return true iff the state has been changed
         */
        bool read();

        /**
         * Returns the current state of the sensor.
         * @return true if light and false if dark
         */
        bool isLight() {
            return !dark;
        }

        /**
         * Returns the current state of the sensor.
         * @return true if dark and false if light
         */
        bool isDark() {
            return dark;
        }

    private:
        int pin;
        bool dark;
        int lastValue;
        unsigned long nextTimestamp;
};

#endif // LightSensor_h
