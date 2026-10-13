#include <TimerOne.h> // install via Library Manager
#include "EMGFilters.h"
#include "blur.h"

#define ADCdebug true

#define SENSOR_INPUT_PIN A0 // input pin number

EMGFilters myFilter;
// discrete filters must works with fixed sample frequency, adjust in constant.h
// For countries where power transmission is at 50 Hz
// For countries where power transmission is at 60 Hz, need to change to NOTCH_FREQ_60"
// our emg filter only support 50Hz and 60Hz input
// other inputs will bypass all the EMG_FILTER

float adjust_factor = 20.0;

// Calibration:
// put on the sensors, and release your muscles;
// wait a few seconds, and select the max value as the throhold;
// any value under throhold will be set to zero
static int Throhold = 0;

unsigned long timeStamp;
unsigned long timeBudget;

const int sampleEvery = 5;
volatile bool sampleReady = false;
volatile int raw = 0;

void sampleISR() {
    raw = analogRead(SENSOR_INPUT_PIN);
    sampleReady = true;
}

constexpr float GBLUR_SIGMA = 1.0;
constexpr int GBLUR_LENGTH = (int)(GBLUR_SIGMA * gb_factor) + 1;
Gaussian_blur<long, GBLUR_LENGTH> gblur;

void setup() {
    myFilter.init(sampleRate, humFreq, true, true, true);
    Serial.begin(9600);
    Timer1.initialize(1000); // 1000 microseconds = 1000 Hz
    Timer1.attachInterrupt(sampleISR);
}

void loop() {
    if (sampleReady) {
        sampleReady = false;
        int filtered = myFilter.filter(raw);
        long envelope = abs((long)filtered) * adjust_factor;
        long blurred = gblur.blur(envelope);

        static int printCounter = 0;
        if (++printCounter >= sampleEvery) {
            printCounter = 0;
            Serial.print("Min: 0, "); 
            Serial.print("Max: 800, "); 

            if (ADCdebug) Serial.println(envelope);
            Serial.println(blurred);
        }
    }
}