# Softwares #

## Overview: ##

Firmware and PC tools for the hand exoskeleton: EMG signal processing, the stepper actuator, and a few test helpers.

```
Electrodes ─► EMG front-end PCB ─► ESP32 ADC (on the breakout board) ─► EMGsignal (filters + rectify + envelope)
                                                                                 ┆
                                                                                 ▼
Buttons / motor_setup.py ─► Arduino ─► (configured)StepperMotor ─► H-bridge ─► bipolar stepper motor ─► actuation
```

The new [EMG integrated sensor](../Electronics/README.md#emg-integrated-sensor-emgintegrated) has its own STM32G431 for on-board processing. Its firmware isn't written yet. The `EMGsignal/src` filter library is board-independent and can be ported to it.

## Projects: ##

| Folder | Board | Description |
|--------|-------|-------------|
| [`EMGsignal/`](EMGsignal) | ESP32 / AVR | Real-time EMG filter library (Butterworth HPF/LPF, mains notch, rectification, Gaussian envelope). Two example sketches: `EMGsignal_ESP.ino` (dual-core FreeRTOS, hardware-timer sampling at 4 kHz) and `EMGsignal_AVR.ino` (`TimerOne` sampling at 2 kHz). Includes a PC test harness. See its [README](EMGsignal/README.md). |
| [`StepperMotor/`](StepperMotor) | Arduino Uno / Nano | Bipolar stepper driver library, a 3-button manual control sketch, and a Python calibration tool. See its [README](StepperMotor/README.md). |
| [`ADCsignalDebug/`](ADCsignalDebug) | Any Arduino | Minimal sketch that prints raw `analogRead(A0)` values at 9600 baud, as fast as the loop runs. Use it to check the wiring and signal level of the front end before running the filters. On ESP32, `A0` is GPIO36, not the GPIO33 that `EMGsignal_ESP.ino` uses, so change the pin to match. |

## Tools you need: ##

| Tool | Used for |
|------|----------|
| Arduino IDE 2.x | Building and uploading all `.ino` sketches |
| esp32 board package (Espressif) **3.x** | `EMGsignal_ESP.ino` (uses the 3.x timer API; doesn't build on 2.x) |
| `TimerOne` library | `EMGsignal_AVR.ino` |
| Python 3 + `matplotlib` | `EMGsignal/test/signal_gen.py` |
| Python 3 + `pyserial`, `keyboard`, `prettier_console` | `StepperMotor/motor_setup/motor_setup.py` |
| A desktop C++ compiler (MSVC `cl` or `g++`) | `EMGsignal/test/filter_exported.cpp` |

### Details refer to each folder's README ###
