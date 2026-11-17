# REPOSITORY FOR EMG FABRIC TEAM PROJECT #

This is the repository for EMG fabric team project of Andy.

Project: hand exoskeleton that uses EMG sensor to detect muscle activity and trigger action in hand (gripping).

```
Electrodes ─► EMG front-end PCB ─► ESP32 on breakout board (filtering + envelope) ─► stepper actuator ─► finger mechanism
```

Next step: the **EMG integrated sensor**, a two-board stack that puts the electrodes, the analog front end and an STM32 microcontroller for signal processing into one small unit:

```
Electrode board ─► EMGintegrated main board (analog front end + STM32G431 DSP) ─► USB-C / UART
```

## [Mechanical](Mechanical/README.md) ##

SolidWorks CAD models for the hand exoskeleton: handpiece, finger linkages with glider and rail, armband, the arm assembly (motor and sensor housings), and earlier design iterations.

Now as personal project going.

## [Electronics](Electronics/README.md) ##

PCB designs (Altium) for the boards used in the exoskeleton:

- **EMGfrontend**: analog-only EMG front end (instrumentation amp, 20–500 Hz band-pass, mid-supply reference).
- **EMGintegrated**: the same analog front end plus an STM32G431, 3.3 V regulator, USB-C and battery input on a 4-layer board, stacked on a separate electrode board. Includes the SolidWorks model of the sensor unit.
- **ESP32 breakout board**: carrier for a Freenove ESP32, with screw terminals for four EMG channels and the motor.
- **Motor test board**: button board for jogging the stepper by hand.

Plus an LTspice simulation of the analog chain.

## [Softwares](Softwares/README.md) ##

The digital signal processing and the motor firmware:

- [EMGsignal](Softwares/EMGsignal/README.md): real-time EMG filter library with ESP32 (dual-core FreeRTOS) and AVR example sketches.
- [StepperMotor](Softwares/StepperMotor/README.md): bipolar stepper driver, button control and calibration tool.
- ADCsignalDebug: raw ADC printout for checking the front end.

### Details please view each module's README. ###
