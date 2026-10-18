# Softwares #

## Overview: ##

Firmware and PC tools for the hand exoskeleton: EMG signal processing, the stepper actuator, and a few test helpers.

```
Electrodes ─► EMG front-end PCB ─► ESP32 ADC ─► EMGsignal (filters + envelope)
                                                         ┆
                                                         ┆ (not connected yet)
                                                         ▼
Buttons / motor_setup.py ─────────────► Arduino ─► StepperMotor ─► H-bridge ─► linear stepper
```

For now, the EMG processing and the motor control are separate programs on separate boards. The EMG envelope does not trigger the motor yet.

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

## Testing the filters on a PC: ##

The filter chain is header-only and board-independent, so it can be tested without hardware. `filter_exported.cpp` runs the same processing as `EMGsignal.ino` on samples read from stdin. `signal_gen.py` generates a test signal, sends it through the program and plots the input against the output.

From inside `EMGsignal/`:

```
mkdir build
cl /EHsc /Fe:build\filter_exported.exe test\filter_exported.cpp     (MSVC)
g++ -O2 -o build/filter_exported.exe test/filter_exported.cpp        (or g++)
python test/signal_gen.py
```

To change the test signal (frequency, amplitude, duration), edit the parameters at the top of `signal_gen.py`. The script's `rate` must match `sample_freq` in `src/constants.h`. If you change one, change the other and recompile.

## Conventions: ##

- Arduino only compiles the files in a sketch's own folder and its `src/` subfolder. Headers next to the `.ino` are included as `#include "file.h"`. Headers in `src/` must be included as `#include "src/file.h"`.
- Python build output (`build/`, `dist/`, `.venv/`, `*.spec`) and the generated `motor_config.json` are git-ignored.
