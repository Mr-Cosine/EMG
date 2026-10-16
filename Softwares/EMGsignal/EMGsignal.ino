#include "Arduino.h"
#include "emg_filters.h"

#define ADCdebug true

#define SENSOR_INPUT_PIN A0 // input pin number (GPIO36 / ADC1_CH0 on ESP32 dev boards)

// Calibration:
// put on the sensors, and release your muscles;
// wait a few seconds, and select the max value as the threshold;
// any value under throhold will be set to zero
static i32 floor_signal = 0;

// How frequent datas are printed (rate = sample_freq / print_every)
const i32 print_every = 10;
i32 print_counter = 0;


uint32_t timeStamp;
volatile bool sampleReady = false;
hw_timer_t* sampleTimer = nullptr;

// analogRead() is not ISR-safe on ESP32, so the ISR only flags the sample
// and loop() does the actual read.
void IRAM_ATTR sampleISR() {
    sampleReady = true;
}

EMGFilters<f32> filter(
    false,  //by pass all
    true,   //notch
    true,   //lpf
    true,   //hpf
    false   //blur
);

void setup() {
    Serial.begin(115200);

    // 10-bit readings (0-1023), same scale as AVR boards
    analogReadResolution(10);

    const uint32_t period_us = (uint32_t)(1000000L / sample_freq);
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    sampleTimer = timerBegin(1000000);              // 1 MHz tick
    timerAttachInterrupt(sampleTimer, &sampleISR);
    timerAlarm(sampleTimer, period_us, true, 0);    // auto-reload, run forever
#else
    sampleTimer = timerBegin(0, 80, true);          // 80 MHz APB / 80 = 1 MHz tick
    timerAttachInterrupt(sampleTimer, &sampleISR, true);
    timerAlarmWrite(sampleTimer, period_us, true);  // auto-reload
    timerAlarmEnable(sampleTimer);
#endif
}

void loop() {
    if (sampleReady) {
        sampleReady = false;
        f32 raw = (f32)(analogRead(SENSOR_INPUT_PIN));

        f32 filtered = filter.process(raw);

        f32 envelope = fabsf(filtered) * amplification_factor - floor_signal;
        if (envelope < 0.0f) envelope = 0.0f;

        i32 blurred = (i32)(filter.gblur().blur(envelope));

        if (++print_counter == print_every) {
            print_counter = 0;

            Serial.print("Min: 0, ");
            Serial.print("Max: 800, ");

            if (ADCdebug) Serial.println(raw);
            else Serial.println(blurred);
        }
    }
}
