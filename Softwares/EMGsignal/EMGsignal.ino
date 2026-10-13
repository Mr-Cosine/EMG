#include "Arduino.h"
#include <TimerOne.h> // install via Library Manager
#include "EMGFilters.h"
#include "blur.h"

#define ADCdebug true

#define SENSOR_INPUT_PIN A0 // input pin number

EMGFilters<double> myFilter;
// discrete filters must works with fixed sample frequency, adjust in constant.h
// For countries where power transmission is at 50 Hz
// For countries where power transmission is at 60 Hz, need to change to NOTCH_FREQ_60"
// our emg filter only support 50Hz and 60Hz input
// other inputs will bypass all the EMG_FILTER

double amplification_factor = 2.0;

// Calibration:
// put on the sensors, and release your muscles;
// wait a few seconds, and select the max value as the throhold;
// any value under throhold will be set to zero
static int Throhold = 0;

unsigned long timeStamp;
unsigned long timeBudget;

const int print_every = 10;
int print_counter = 0;

volatile bool sampleReady = false;
volatile double raw = 0;

void sampleISR() {
    raw = (double)(analogRead(SENSOR_INPUT_PIN));
    sampleReady = true;
}

constexpr double GBLUR_SIGMA = 1.0;
constexpr int GBLUR_LENGTH = (int)(GBLUR_SIGMA * gb_factor) + 1;
Gaussian_blur<double, GBLUR_LENGTH> gblur(true, double(0));

void setup() {
    myFilter.init(false, true, true, true);
    Serial.begin(115200);
    Timer1.initialize((int)(1e6 / sample_freq));
    Timer1.attachInterrupt(sampleISR);
}

void loop() {
    if (sampleReady) {
        sampleReady = false;
        double filtered = myFilter.process(raw);
        double envelope = abs((double)filtered) * amplification_factor;
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