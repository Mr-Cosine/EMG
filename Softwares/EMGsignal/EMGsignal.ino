#include "Arduino.h"
#include <TimerOne.h> // install via Library Manager
#include "EMGFilters.h"
#include "blur.h"

#define ADCdebug true

#define SENSOR_INPUT_PIN A0 // input pin number

// Calibration:
// put on the sensors, and release your muscles;
// wait a few seconds, and select the max value as the threshold;
// any value under throhold will be set to zero
static int floor_signal = 0;

unsigned long timeStamp;

// How frequent datas are printed (rate = sample_freq / print_every)
const int print_every = 10;
int print_counter = 0;

volatile bool sampleReady = false;
volatile double raw = 0;

void sampleISR() {
    raw = (double)(analogRead(SENSOR_INPUT_PIN));
    sampleReady = true;
}

EMGFilters<double> myFilter;

static constexpr int GBLUR_LENGTH = (int)(2 * GBLUR_SPAN) + 1;
Gaussian_blur<double, GBLUR_LENGTH> gblur{true, double(0)};

void setup() {
    myFilter.init(false, true, true, true);
    Serial.begin(115200);
    Timer1.initialize((int)(1e6 / sample_freq - process_time_budget));
    Timer1.attachInterrupt(sampleISR);
}

void loop() {
    if (sampleReady) {
        sampleReady = false;
        double filtered = myFilter.process(raw);

        double envelope = abs((double)filtered) * amplification_factor - floor_signal;
        if (envelope < 0) envelope = 0.0;

        long blurred = (long)(gblur.blur(envelope));
        
        if (++print_counter == print_every) {
            print_counter = 0;

            Serial.print("Min: 0, "); 
            Serial.print("Max: 800, "); 

            if (ADCdebug) Serial.println(raw);
            else Serial.println(blurred);
        }
    }
}