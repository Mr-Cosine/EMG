
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif

#define TIMING_DEBUG 0

#define SensorInputPin A0 // input pin number

void setup() {
    // open serial
    Serial.begin(9600);
}

void loop() {
    Serial.println(analogRead(SensorInputPin));
}
