# REPOSITORY FOR EMG FABRIC TEAM PROJECT #

This is the repository for EMG fabric team project of Andy.

Project: hand exoskeleton that uses EMG sensor to detect muscle activity and trigger action in hand (gripping).

```
Electrodes ─► EMG front-end PCB ─► ESP32 (filtering + envelope) ─► stepper actuator ─► finger mechanism
```

## [Mechanical](Mechanical/README.md) ##

SolidWorks CAD models for the hand exoskeleton: handpiece, finger linkages with glider and rail, armband, and earlier design iterations.

Now as personal project going.

## [Electronics](Electronics/README.md) ##

PCB designs (Altium) for the boards used in the exoskeleton: the EMG sensor analog front end and the motor test button board, plus LTspice simulation of the analog chain.

## [Softwares](Softwares/README.md) ##

The digital signal processing and the motor firmware:

- [EMGsignal](Softwares/EMGsignal/README.md): real-time EMG filter library and ESP32 sketch.
- [StepperMotor](Softwares/StepperMotor/README.md): bipolar stepper driver, button control and calibration tool.
- ADCsignalDebug: raw ADC printout for checking the front end.

### Details please view each module's README. ###
