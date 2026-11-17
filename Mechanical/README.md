# Mechanical #

## Overview: ##

SolidWorks CAD for the project. There are two projects:

- **Wearable hand exoskeleton** (main project): a handpiece on the back of the hand, finger mechanisms driven by a glider sliding on a rail, and the **arm assembly**, a forearm band that carries the stepper actuator and the EMG sensor electronics. The glider is moved by the linear stepper actuator (firmware in [`Softwares/StepperMotor`](../Softwares/StepperMotor)) to close and open the hand.
- **Arm** (separate project): a hand model with a palm and jointed fingers, including the thumb.

This part of the project is currently developed as a personal project.

## Folder layout: ##

```
Mechanical/
├── Wearable hand exoskeleton/
│   ├── new hand - metal glider v5.SLDASM   Hand assembly (open this)
│   ├── handpiece v5.SLDPRT                 Handpiece used in the v5 assembly
│   ├── handpiece v6.SLDPRT                 Newest handpiece (work in progress)
│   ├── armband_base.SLDPRT                 Forearm armband base
│   ├── Fits.txt                            Fit clearances for the hand (SolidWorks equations)
│   ├── finger/                             Finger mechanism parts and assemblies
│   ├── Arm assembly/                       Forearm band with actuator and EMG electronics
│   ├── CAD drawing/                        Manufacturing drawings
│   └── legacy files/                       Earlier design iterations
│         ├── v1
│         ├── v2
│         ├── v3
│         ├── v4
│         └── v5
└── Arm/
    └── Hand/                               Hand model (palm, fingers, thumb)
```

## Wearable hand exoskeleton: ##

### `finger/` ###

| File | Description |
|------|-------------|
| `index&ring finger v4.SLDASM` | Finger assembly for the index and ring finger |
| `middle finger v4.SLDASM`     | Finger assembly for the middle finger |
| `pinky nail v4.SLDASM`        | Finger assembly for the pinky |
| `beam v5.SLDPRT`              | Linkage beam |
| `connector v5.SLDPRT`, `connector - big v5.SLDPRT`, `connector - tip  v5.SLDPRT` | Linkage connectors (standard, large, fingertip) |
| `central support v5.SLDPRT`   | Central support of the finger linkage |
| `new ring v5.SLDPRT`          | Ring that holds the mechanism on the finger |
| `glider - metal.SLDPRT`, `glider base.SLDPRT`, `glider fix.SLDPRT` | Sliding glider driven by the actuator, its base and its fixing |
| `glider base_4_CNC.SLDPRT`    | Glider base simplified for CNC machining |
| `rail.SLDPRT`                 | Rail the glider slides on |

The finger assemblies keep their `v4` file names, but they use the `v5` parts.

### `Arm assembly/` ###

The forearm part of the exoskeleton: a band around the forearm with two housings, one for the actuator and one for the EMG electronics.

| File | Description |
|------|-------------|
| `total_assembly.SLDASM`  | Full arm assembly: band + motor assembly + sensor assembly (open this) |
| `band.SLDPRT`            | Forearm band |
| `motor_assembly.SLDASM`  | Actuator housing assembly |
| `motor.SLDPRT`           | Linear stepper actuator |
| `CH-SM1545.step`         | Vendor model of the CH-SM1545 linear stepper (lead screw and slider; coil wires A red, Ā white, B brown, B̄ black) |
| `motor driver.SLDPRT`    | Stepper driver board |
| `motor_shell.SLDPRT`     | Housing for the motor and driver |
| `rotator.SLDPRT`, `rotator_cap.SLDPRT` | Rotator and its cap |
| `sensor_assembly.SLDASM` | Electronics housing assembly |
| `sensor_shell.SLDPRT`    | Housing for the EMG electronics |
| `EMGfrontend.SLDPRT`     | Model of the EMG front-end PCB ([`Electronics/.../EMGfrontend`](../Electronics/README.md#emg-front-end-emgfrontend)) |
| `Breakout_board.step`    | STEP export of the ESP32 breakout PCB ([`Electronics/Breakout-Freenove ESP32`](../Electronics/README.md#esp32-breakout-board)) |
| `constants.txt`          | Shared dimensions and fits for the arm assembly (SolidWorks equations, see below) |

The board models come from the Altium projects in [`Electronics`](../Electronics/README.md). If a board outline or connector position changes, re-export the board and replace the file here. The SolidWorks model of the integrated EMG sensor (board stack + shell) is kept with its PCB, in [`Electronics/EMG sensor front end/layout/EMGintegrated/Assembly/unit.SLDASM`](../Electronics/EMG%20sensor%20front%20end/layout/EMGintegrated/Assembly).

`PerformanceLog.txt` is written by SolidWorks. It isn't a design file.

### `CAD drawing/` ###

| File | Description |
|------|-------------|
| `glider base_4_CNC_easy.SLDDRW` | 2D drawing of the CNC glider base |
| `sensor_shell_cap.SLDDRW`       | 2D drawing of the cap of the EMG sensor shell |

## Fits and shared dimensions: ##

The shared values are stored as SolidWorks equation files.

`Fits.txt` (hand and fingers):

| Variable | Value   | Meaning |
|----------|---------|---------|
| `LF`     | 0.04 mm | Loose fit (parts that must move freely) |
| `TF`     | 0.02 mm | Tight fit (parts that must hold in place) |
| `AngLF`  | 0.5°    | Angular loose fit |
| `AngTF`  | 0.3°    | Angular tight fit |

`Arm assembly/constants.txt` (arm assembly):

| Variable    | Value  | Meaning |
|-------------|--------|---------|
| `bandwidth` | 20 mm  | Width of the forearm band |
| `loosefit`  | 0.5 mm | Loose fit |
| `tightfit`  | 0.2 mm | Tight fit |

To use them in a part, go to **Tools › Equations › Link to external file** and select the file. Then use `"LF"`, `"bandwidth"` and so on in dimensions. If you change a value in the file, every linked part updates on rebuild.

## Design history (`legacy files/`): ##

| Version | Assembly | Main change |
|---------|----------|-------------|
| v1 | `hand.SLDASM` | First hand: fixed position for each finger. Flexibility comes from the beam's elasticity. Includes the thumb |
| v2 | `new hand - normal glider.SLDASM` | Changed to a sliding finger base for comfort in movement. Square 3D-printed glider and slot |
| v3 | `new hand - circular glider.SLDASM` | Circular-profile 3D-printed glider |
| v4 | `new hand - metal glider (v4).SLDASM` | Changed to a metal glider and rail |
| v5 | `new hand - metal glider v5.SLDASM` | Handpiece split into an upper and a lower part. The active design for now (v6 is still under development) |

`legacy files/armpiece.SLDPRT` is the first forearm part. It was replaced by `armband_base.SLDPRT`, and the forearm is now designed in `Arm assembly/`.

Older versions are kept for reference only. Don't edit them. To start a new version, copy the current files into `legacy files/vN/` first, then work on the top-level files.

## Arm (`Arm/Hand/`): ##

A separate hand model.

| File | Description |
|------|-------------|
| `Hand.SLDASM`         | Full hand assembly (open this) |
| `palm.SLDPRT`         | Palm |
| `Finger.SLDASM`       | Finger assembly |
| `Finger_smal.SLDASM`  | Smaller finger assembly |
| `Finger_thumb.SLDASM` | Thumb assembly |
| `Finger_node.SLDPRT`  | Finger segment (node) used by the finger assemblies |

## Notes: ##

- Open the `.SLDASM` files from inside their own folder structure. SolidWorks resolves parts by relative path, so moving or renaming a part breaks the assembly references.
- The hand files were moved into `Wearable hand exoskeleton/`. If a file prompts for a missing `Fits.txt` or `constants.txt`, browse to the file in its current folder to replace the missing reference.
- Each folder's `.gitignore` excludes SolidWorks lock files (`~$*`). Close SolidWorks before committing, so no file is still open.
