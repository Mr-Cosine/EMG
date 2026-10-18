# Softwares #

## Overview: ##

Firmware and PC tools for the hand exoskeleton: EMG signal processing, the stepper actuator, and a few test helpers.

```
Electrodes ─► EMG front-end PCB ─► ESP32 ADC ─► EMGsignal (filters + envelope)
                                                         ┆
                                                         ▼
Buttons / motor_setup.py ─► Arduino ─► (configured)StepperMotor ─► H-bridge ─► bipolar stepper motor ─► actuation
```

## Projects: ##

| Folder | Board | Description |
|--------|-------|-------------|
| [`EMGsignal/`](EMGsignal) | ESP32 | Real-time EMG filter library (Butterworth HPF/LPF, mains notch, Gaussian envelope) and an example sketch that samples at a fixed rate with a hardware timer. Includes a PC test harness. See its [README](EMGsignal/README.md). |
| [`StepperMotor/`](StepperMotor) | Arduino Uno / Nano | Bipolar stepper driver library, a 3-button manual control sketch, and a Python calibration tool. See its [README](StepperMotor/README.md). |
| [`ADCsignalDebug/`](ADCsignalDebug) | Any Arduino | Minimal sketch that prints raw `analogRead(A0)` values at 9600 baud, as fast as the loop runs. Use it to check the wiring and signal level of the front end before running the filters. |

## Tools you need: ##

| Tool | Used for |
|------|----------|
| Arduino IDE 2.x | Building and uploading all `.ino` sketches |
| esp32 board package (Espressif, 2.x or 3.x) | `EMGsignal.ino` |
| Python 3 + `matplotlib` | `EMGsignal/test/signal_gen.py` |
| Python 3 + `pyserial`, `keyboard`, `prettier_console` | `StepperMotor/motor_setup/motor_setup.py` |
| A desktop C++ compiler (MSVC `cl` or `g++`) | `EMGsignal/test/filter_exported.cpp` |

### Details refer to each folder's README ###