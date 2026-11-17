# Electronics #

## Overview: ##

PCB designs and circuit simulations for the hand exoskeleton. There are four boards:

- **EMG front end (`EMGfrontend`)**: an analog-only board. It amplifies the surface EMG signal from the electrodes, band-limits it to 20–500 Hz, and centres it at mid-supply so a microcontroller ADC can sample it. Electrodes plug in through a 3.5 mm jack, and the analog output goes out on a 3-pin connector. The firmware that processes its output is in [`Softwares/EMGsignal`](../Softwares/EMGsignal).
- **EMG integrated sensor (`EMGintegrated`)**: the next version. It is a compact two-board stack. The main board carries the same analog front end plus an **STM32G431** microcontroller for on-board digital signal processing, a 3.3 V regulator, USB-C and a battery input. Below it sits an electrode board with the skin-contact electrodes.
- **ESP32 breakout board**: a carrier board for a Freenove ESP32 dev board, with screw terminals for four EMG channels and the motor, plus push-in terminal blocks for general wiring.
- **Motor test board**: a small button board (forward / backward / speed) used to jog the stepper actuator by hand. Its firmware is [`Softwares/StepperMotor`](../Softwares/StepperMotor).

The PCBs are designed in **Altium Designer**. The analog chain is simulated in **LTspice**. Enclosure and stack CAD is in **SolidWorks**.

## Folder layout: ##

```
Electronics/
├── EMG sensor front end/
│   ├── layout/
│   │   ├── EMGfrontend/                      Analog-only front-end board
│   │   │   ├── EMGfrontend.PrjPcb            Project file (open this)
│   │   │   ├── EMGfrontend.SchDoc            Top sheet: electrode jack, output connector, LED, output RC, sheet symbols
│   │   │   ├── VREF.SchDoc                   Mid-supply reference
│   │   │   ├── AMP1.SchDoc                   Instrumentation amplifier
│   │   │   ├── HPF(20Hz).SchDoc              20 Hz high-pass filter
│   │   │   ├── AMP2.SchDoc                   Second gain stage (with gain trimpot)
│   │   │   ├── LPF(500Hz).SchDoc             500 Hz low-pass filter
│   │   │   ├── EMGfrontend.PcbDoc            Board layout (2 layers)
│   │   │   └── BOM_EMGfrontendBomDoc.BomDoc  ActiveBOM
│   │   └── EMGintegrated/                    Integrated sensor: analog front end + STM32 DSP
│   │       ├── EMGintegrated.PrjPcb          Project file (open this)
│   │       ├── EMGintegrated.SchDoc          Top sheet: MCU, USB-C, UART, button, electrode header
│   │       ├── Powereg.SchDoc                Power: USB / battery input and 3.3 V LDO
│   │       ├── EMGfrontend.SchDoc            Analog front-end sub-hierarchy (input protection, output RC)
│   │       ├── VREF / AMP1 / HPF(20Hz) / AMP2 / LPF(500Hz) .SchDoc   Analog stages
│   │       ├── EMGintegrated.PcbDoc          Main board layout (4 layers)
│   │       ├── EMGintegrated.BomDoc          ActiveBOM
│   │       ├── Electrode/Electrode/          Electrode board project (bottom of the stack)
│   │       │   ├── Electrode.PrjPcb          Project file (open this)
│   │       │   ├── electrode.SchDoc
│   │       │   └── electrode.PcbDoc          Board layout (2 layers)
│   │       └── Assembly/                     SolidWorks model of the sensor unit
│   │           ├── unit.SLDASM               Board stack + spacer + shell (open this)
│   │           ├── EMGintegrated.SLDPRT, electrode.SLDPRT
│   │           └── spacer.SLDPRT, shell.SLDPRT
│   └── simulation/
│       └── Virtual Ground.asc                LTspice model of the full analog chain
├── Breakout-Freenove ESP32/
│   └── PCB_Project/
│       ├── PCB_Project.PrjPcb                Project file (open this)
│       ├── Breakout_board.SchDoc             Schematic
│       ├── Breakout_board.PcbDoc             Board layout (2 layers)
│       ├── PCB_Project.BomDoc                ActiveBOM
│       └── *.SchLib / PHOENIX_*.PcbLib       Phoenix Contact terminal symbols and footprints
└── motor test/
    └── PCB_testboard/
        ├── Sheet1.SchDoc                     Schematic of the button board
        ├── PCB_testboard.PrjPcb              Project file (open this)
        └── PCB1.PcbDoc                       Board layout
```

## EMG analog front end: ##

Both `EMGfrontend` and `EMGintegrated` use the same analog chain. The differences are listed in [EMG integrated sensor](#emg-integrated-sensor-emgintegrated).

### Signal chain ###

The analog circuit runs from a single **3.3 V** supply. Every stage is referenced to `VREF` (≈ 1.65 V, mid-supply), so the single-supply op-amps can handle an EMG signal that swings both above and below zero.

```
Electrodes  3.5 mm jack (EMGfrontend) / 4-pin header from the electrode board (EMGintegrated)
   ↓
Input       5 kΩ series resistors, 10 MΩ bias resistors, ferrite bead
   ↓
AMP1        Instrumentation amp, AD8226, G = 1 + 49.4 kΩ / 15 kΩ ≈ 4.3
   ↓
HPF         2nd-order Sallen-Key high-pass, fc ≈ 20 Hz
   ↓
AMP2        Non-inverting amp
   ↓
LPF         2nd-order Sallen-Key low-pass, fc ≈ 500 Hz
   ↓
LPF2        Passive RC filter (470 Ω, 22 nF), fc ≈ 15 kHz
   ↓
OUT ── EMGfrontend: 1×3 JST GH 1.25 mm connector / EMGintegrated: STM32 ADC
```

- **Gain of AMP1** follows the AD8226 formula `G = 1 + 49.4 kΩ / RG`. Change `RG` (`R3`) on `AMP1.SchDoc` to change the front-end gain.
- **Gain of AMP2** is `1 + Rf / 1 kΩ`:
  - `EMGfrontend`: `Rf` = 39 kΩ + 20 kΩ trimpot (`Rv1`), so the gain is adjustable from about 40 to 60. At ≈ 52 the total gain is ≈ 4.3 × 52 ≈ **223 (≈ 47 dB)**, and the output clips at roughly ±7 mV differential input.
  - `EMGintegrated`: `Rf` = 62 kΩ fixed (no trimpot), so the gain is ≈ 63. The total gain is ≈ 4.3 × 63 ≈ **270 (≈ 49 dB)**, and the output clips at roughly ±6 mV differential input.
- **Both active filters are Butterworth (Q ≈ 0.7)**. The digital filters in `Softwares/EMGsignal` use the same cutoffs, which sharpens the band edges and removes mains hum.

### Sheets ###

| Sheet               | Function |
|---------------------|----------|
| `EMGfrontend.SchDoc`| Top sheet (EMGfrontend) or analog sub-sheet (EMGintegrated): input protection and bias, sheet links, output RC |
| `VREF.SchDoc`       | Buffered mid-supply reference |
| `AMP1.SchDoc`       | Differential-to-single-ended amplification |
| `HPF(20Hz).SchDoc`  | Removes DC offset and motion artefacts |
| `AMP2.SchDoc`       | Main gain stage |
| `LPF(500Hz).SchDoc` | Anti-aliasing low-pass |

Unless noted otherwise, all passives are 0603 SMD. The full part list with manufacturer numbers is in each project's ActiveBOM.

### Simulation (`simulation/`) ###

`Virtual Ground.asc` models the whole chain, from VREF to the ADC input:

- It uses the LTspice `AD8226` model and the generic `UniversalOpAmp2` in place of the MCP6001/MCP6002.
- The input is a 7 mV **chirp** (`B1`) that sweeps from 10 Hz to about 10 kHz over the 0.5 s transient (`.tran 0.5`). Use it to check the pass band.
- The schematic also contains spare test sources: 1.4 mV sines at 125, 250, 500, 1000 and 2000 Hz (`V2`–`V6`) and a 7 mV 200 Hz sine (`V7`). Connect one of them to `SIGNAL` in place of `B1` to test a single frequency.
- Useful probe nodes: `AMPOUT` (after AMP1), `RCHPFOUT` (after HPF), `GAINOUT` (after AMP2), `PROCESSED` (after LPF), `ADC` (board output).

A few values differ slightly from the PCBs: the LPF capacitor is 20 nF in the simulation and 22 nF on the boards, the VREF output resistor is 10 Ω in the simulation and 22 Ω on the boards, and the simulation models the `EMGfrontend` AMP2 gain, not the fixed 62 kΩ of `EMGintegrated`.

## EMG front end (`EMGfrontend`): ##

The first, analog-only board (about 45 × 34 mm, 2 layers). It needs an external 3.3 V supply and an external microcontroller (the ESP32 on the breakout board).

| Designator | Part | Function |
|------------|------|----------|
| `SGNL`     | CUI SJ2-3574A-SMT-TR, 3.5 mm 4-pole jack | Electrode cable input |
| `ARD`      | JST SM03B-GHS-TB, 3-pin 1.25 mm | Power in and analog output to the microcontroller |
| `D1`, `RL` | Red LED + 620 Ω | Power indicator |
| `U1`, `U3` | MCP6002 (dual op-amp) | `U1`: VREF + HPF, `U3`: AMP2 + LPF |
| `U2`       | AD8226 | Instrumentation amplifier (AMP1) |
| `Rv1`      | Bourns 3313J 20 kΩ trimpot | AMP2 gain adjustment |

## EMG integrated sensor (`EMGintegrated`): ##

A self-contained sensor unit: the analog front end and an STM32 microcontroller for digital signal processing, built as a **two-board stack** of about **36 × 23 mm**:

```
┌──────────────────────────────────────────────┐
│ EMGintegrated main board (4 layers)          │  analog front end + STM32G431 + power + USB-C / UART
│ Signal1 / Ground / Power / Signal2           │
└──────────────┬───────────────────────────────┘
               │  J1: 4-pin 2.54 mm header (+IN, REF, −IN, GND)
┌──────────────┴───────────────────────────────┐
│ Electrode board (2 layers)                   │  skin-contact electrodes
└──────────────────────────────────────────────┘
                     skin
```

The two boards share the same outline. They are joined by a Samtec HTSW-104 header on the main board and an SSW-104 socket on the electrode board, with a spacer between them. The whole unit sits in a shell (see `Assembly/unit.SLDASM`).

### Main board ###

| Block | Designators | Parts | Notes |
|-------|-------------|-------|-------|
| Microcontroller | `MCU1`, `Cd4`–`Cd7`, `L2` | STM32G431KBU6 (Arm Cortex-M4F with FPU, 170 MHz, 128 KB flash, UFQFPN32) | Samples the analog output and runs the digital filters. 100 nF + 4.7 µF decoupling, ferrite bead `L2` on the supply |
| Analog front end | `U1A`, `U1B`, `U2`, `U3` | 2 × MCP6001, AD8226, MCP6002 | Same chain as above. VREF and HPF each use a single MCP6001 (SOT-23-5) instead of one shared MCP6002 |
| Electrode connector | `J1` | Samtec HTSW-104-07-G-S, 4-pin 2.54 mm | To the electrode board: `+IN`, `REF`, `−IN`, `GND` |
| USB-C | `J3`, `Rc1`, `Rc2`, `D4`–`D6` | 12401610E4#2A receptacle, 5.1 kΩ CC pull-downs, SZESD8472 TVS diodes | 5 V input (the CC pull-downs make it a USB-C sink). TVS diodes for ESD protection |
| Battery | `J2` | JST S2B-PH-SM4-TB, 2-pin 2.0 mm side-entry | Battery input (`VBAT`) |
| Power regulator | `U0`, `D2`, `D3`, `Cv2`–`Cv4` (`Powereg.SchDoc`) | TI LP2985-33 (3.3 V, 150 mA LDO), 2 × 1PS79SB30 Schottky | `VBUS` and `VBAT` are diode-ORed into the LDO, so the board runs from either USB or the battery |
| UART | `UART` | JST SM04B-GHS-TB, 4-pin 1.25 mm | `VCC`, `RX`, `GND`, `TX` |
| Button | `S1` | C&K KMR221GLFS tactile switch | |
| Divider | `Ra1`, `Ra2`, `Ca1` | 100 kΩ / 39 kΩ, 100 nF | |
| Switch circuit | `Q6`, `Rs1`, `Rs2`, `Cs1`, `Cs2`, `D7` | 2N7002 N-MOSFET, 10 kΩ, 470 kΩ, 100 nF, 4.7 µF, 1PS79SB30 | |

Compared with `EMGfrontend`, the integrated board drops the 3.5 mm jack, the 3-pin output connector, the power LED and the gain trimpot.

### Electrode board ###

| Designator | Part | Function |
|------------|------|----------|
| `Electrode`| Custom footprint | Skin-contact electrode pads: `+IN`, `−IN`, `REF` |
| `J1`       | Samtec SSW-104-01-G-S, 4-pin 2.54 mm socket | Plugs onto `J1` of the main board |

### Firmware ###

There is no STM32 firmware in the repository yet. The filter library in [`Softwares/EMGsignal/src`](../Softwares/EMGsignal/src) is board-independent, so it can be ported to the STM32G431.

## ESP32 breakout board: ##

A carrier board (about 56 × 74 mm, 2 layers) for a **Freenove ESP32** dev board. The dev board plugs into two sockets, and every external connection goes to screw or push-in terminals instead of jumper wires.

| Designator | Part | Function |
|------------|------|----------|
| `board1`, `board2` | Samtec SSW-120-01-G-S, 20-pin 2.54 mm sockets | Sockets for the two pin rows of the Freenove ESP32 |
| `EMG1`–`EMG4` | Phoenix Contact 1725669, 3-position 2.54 mm screw terminal | Four EMG sensor channels (3-wire: power, ground, signal, matching the `EMGfrontend` output connector) |
| `motor` | Phoenix Contact 1725698, 6-position 2.54 mm screw terminal | Motor / motor driver connection |
| `J1`–`J4` | Phoenix Contact 1771004, 7-position 2.5 mm push-in terminal | Additional wiring |

For the exact ESP32 pin assigned to each terminal, see `Breakout_board.SchDoc`. The ESP32 firmware ([`EMGsignal_ESP.ino`](../Softwares/EMGsignal/EMGsignal_ESP.ino)) reads the EMG signal on **GPIO33**.

A STEP export of the board for the mechanical assembly is in [`Mechanical/Wearable hand exoskeleton/Arm assembly/Breakout_board.step`](../Mechanical/Wearable%20hand%20exoskeleton/Arm%20assembly/Breakout_board.step).

## Motor test board: ##

A button board for manual control of the stepper actuator.

| Designator | Part | Function |
|------------|------|----------|
| `FWD`      | TE 1825910-6 tactile switch | Move forward |
| `BWD`      | TE 1825910-6 tactile switch | Move backward |
| `SPD-`     | TE 1825910-6 tactile switch | Next speed level |
| `J?`       | JST B4B-XH-A, 4-pin 2.5 mm header | 3 buttons + GND to the Arduino |

The buttons connect to GND and rely on the Arduino's internal pull-ups, so the board has no resistors. With the default firmware, they go to D7 (`FWD`), D8 (`BWD`) and D9 (`SPD`). See the wiring table in [`Softwares/StepperMotor/README.md`](../Softwares/StepperMotor/README.md).

The schematic `Sheet1.SchDoc` is one folder above the project (`..\Sheet1.SchDoc`). Keep that relative path if you move the files.

## Opening the projects: ##

1. Open the `.PrjPcb` file in Altium Designer, not the individual `.SchDoc` / `.PcbDoc`. Otherwise the sheets aren't linked and the hierarchy, net names and BOM don't resolve.
2. Several footprints and symbols come from SnapEDA / Octopart / the manufacturers. Most are embedded in the documents. The project-local libraries (`*.SchLib`, `*.PcbLib`) sit next to each project file. Keep them there.
3. The electrode board is a separate project (`EMGintegrated/Electrode/Electrode/Electrode.PrjPcb`). Change both boards together if you change the stack outline or the 4-pin connector.
4. Open `simulation/Virtual Ground.asc` in LTspice and press **Run**.
5. Open `EMGintegrated/Assembly/unit.SLDASM` in SolidWorks to see the board stack in its shell.
