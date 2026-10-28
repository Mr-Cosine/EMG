#include <TimerOne.h> // install via Library Manager
#include "src/emg_filters.h"

#define ADCdebug true

#define SENSOR_INPUT_PIN A0 // input pin number

EMGFilters<float> myFilter(
    false,
    true,
    true,
    true,
    true
);

unsigned long timeStamp;
unsigned long timeBudget;

const int sampleEvery = 10;
volatile bool sampleReady = false;

volatile int adc = 0;

const float adjustment_factor = 10.0;

int raw = 0;
float filtered = 0;
float envelope = 0;

void sampleISR() {
    adc = analogRead(SENSOR_INPUT_PIN);
    sampleReady = true;
}

void setup() {
    Serial.begin(115200);
    Timer1.initialize(500);
    Timer1.attachInterrupt(sampleISR);
}

void loop() {
    noInterrupts();
    raw = adc;
    interrupts();

    if (sampleReady) {
        sampleReady = false;
        filtered = myFilter.process((float)raw);
        envelope = abs(filtered) * adjustment_factor;

        static int printCounter = 0;
        if (++printCounter >= sampleEvery) {
            printCounter = 0;
            Serial.print("Min:0, ");
            Serial.print("Max:200, ");

            Serial.print("Signal:");
            Serial.println(envelope);
        }
    }
}