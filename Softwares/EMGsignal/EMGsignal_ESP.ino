#include "src/emg_filters.h"

#define ADCdebug true

#define SENSOR_INPUT_PIN 33 // input pin number

volatile bool new_sample = false;
volatile _int adc = 0;

_short printCounter = 0;
const _int printEvery = 5;

hw_timer_t *timer = nullptr;
TaskHandle_t sampleTask;

void IRAM_ATTR onTimer() {
  BaseType_t woken = pdFALSE;
  vTaskNotifyGiveFromISR(sampleTask, &woken);
  if (woken) portYIELD_FROM_ISR();
}

void sample(void*) {
    while(true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        adc = analogRead(SENSOR_INPUT_PIN);
        new_sample = true;
    }
}

inline void print_data(_float envelope) {
    if (++printCounter >= printEvery) {
        printCounter = 0;
        Serial.print("Min:0, ");
        Serial.print("Max:400, ");

        Serial.print("Signal:");
        Serial.println(envelope);
    }
}

void process(void*) {
    EMGFilters<_float> myFilter(
        false,
        true,
        true,
        true,
        true
    );

    _int raw = 0;
    _float filtered = 0;
    _float envelope = 0;

    const _float adjustment_factor = 5.0;
    
    while(true) {
        if (!new_sample) continue;
        raw = adc;
        new_sample = false;

        filtered = myFilter.process((_float)raw);
        envelope = abs(filtered) * adjustment_factor;
        
        print_data(envelope);
    }
}

void setup() {
    Serial.begin(460800);
    xTaskCreatePinnedToCore(sample, "sample", 1024, NULL, 2, &sampleTask, 0); //Core 0 for sampling and motor.
    xTaskCreatePinnedToCore(process, "process", 4096, NULL, 1, NULL, 1); //Hog core 1 for processing entirely.

    timer = timerBegin(1000000);                 // 1 MHz tick = 1 µs units
    timerAttachInterrupt(timer, &onTimer);
    timerAlarm(timer, 1000000 / sample_freq, true, 0);
}

void loop() { vTaskDelete(NULL); }