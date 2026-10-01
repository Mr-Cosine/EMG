#include <TimerOne.h> // install via Library Manager
#include "EmgFilters.h"
#include "blur.h"

#define ADCdebug true

#define SENSOR_INPUT_PIN A0 // input pin number

EMGFilters myFilter;
// discrete filters must works with fixed sample frequence
// our emg filter only support "SAMPLE_FREQ_500HZ" or "SAMPLE_FREQ_1000HZ"
// other sampleRate inputs will bypass all the EMG_FILTER
int sampleRate = 1000;
// For countries where power transmission is at 50 Hz
// For countries where power transmission is at 60 Hz, need to change to
// "NOTCH_FREQ_60"
// our emg filter only support 50Hz and 60Hz input
// other inputs will bypass all the EMG_FILTER
int humFreq = 60;

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

gaussian_blur<float> gblur;
const float GBLUR_SIGMA = 1.0;
gblur.create_kernel(GBLUR_SIGMA);
Queue<long, glur.window_len()> inputWindow;

void setup() {
    myFilter.init(sampleRate, humFreq, false, false, true);
    Serial.begin(9600);
    Timer1.initialize(1000); // 1000 microseconds = 1000 Hz
    Timer1.attachInterrupt(sampleISR);
    
    for (int i = 0; i < inputWindow.size(); i++) {
        inputWindow.update(0.0);
    }
}

void loop() {
    if (sampleReady) {
        sampleReady = false;
        int filtered = raw;
        long envelope = abs((long)filtered) * adjust_factor;
        envelope = (envelope > Throhold) ? envelope : 0;

        static int printCounter = 0;
        if (++printCounter >= sampleEvery) {
            printCounter = 0;
            Serial.print("Min: 0, "); 
            Serial.print("Max: 800, "); 

            if (ADCdebug) Serial.println(envelope);
            inputWindow.update(envelope);
            Serial.println(gblur.blur(inputWindow));
        }
    }
}