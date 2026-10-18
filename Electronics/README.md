# Electronics #

## Overview: ##

PCB designs and circuit simulations for the hand exoskeleton. There are two boards:

- **EMG sensor front end**: an analog board that amplifies the surface EMG signal from the electrodes, band-limits it to 20–500 Hz, and centres it at mid-supply so the ESP32 ADC can sample it. The firmware that processes its output is in [`Softwares/EMGsignal`](../Softwares/EMGsignal).
- **Motor test board**: a small button board (forward / backward / speed) used to jog the stepper actuator by hand. Its firmware is [`Softwares/StepperMotor`](../Softwares/StepperMotor).

The PCBs are designed in **Altium Designer**. The analog chain is simulated in **LTspice**.

## Folder layout: ##

```
Electronics/
├── EMG sensor front end/
│   ├── layout/PCB_Project/               Altium project for the EMG front-end board
│   │   ├── EMGfrontend.PrjPcb            Project file (open this)
│   │   ├── EMGfrontend.SchDoc            Top sheet: connectors, LED, output RC, sheet symbols
│   │   ├── VREF.SchDoc                   Mid-supply reference
│   │   ├── AMP1.SchDoc                   Instrumentation amplifier
│   │   ├── HPF(20Hz).SchDoc              20 Hz high-pass filter
│   │   ├── AMP2.SchDoc                   Second gain stage
│   │   ├── LPF(500Hz).SchDoc             500 Hz low-pass filter
│   │   ├── EMGfrontend.PcbDoc            Board layout
│   │   └── BOM_EMGfrontendBomDoc.BomDoc  ActiveBOM
│   └── simulation/
│       └── Virtual Ground.asc            LTspice model of the full analog chain
└── motor test/
    └── PCB_testboard/
        ├── Sheet1.SchDoc                 Schematic of the button board
        ├── PCB_testboard.PrjPcb          Project file (open this)
        └── PCB1.PcbDoc                   Board layout
```

## EMG sensor front end: ##

### Signal chain ###

The board runs from a single **3.3 V** supply. Every stage is referenced to `VREF` (≈ 1.65 V, mid-supply), so the single-supply op-amps can handle an EMG signal that swings both above and below zero.

```
Electrodes  3.5 mm jack
   ↓
AMP1        Instrumentation amp, AD8226,
   ↓
HPF         2nd-order Sallen-Key high-pass, fc ≈ 20 Hz
   ↓
AMP2        Non-inverting amp
   ↓
LPF         2nd-order Sallen-Key low-pass, fc ≈ 500 Hz 
   ↓
LPF2        passive RC filter fc = 15000 Hz
   ↓
OUT ── 1×3 JST ph2.0 connector
```

- **Total gain** ≈ 4.3 × 52 ≈ **223 (≈ 47 dB)**. With a 3.3 V supply, the output clips at roughly ±7 mV differential input at the electrodes.
- **Gain of AMP1** follows the AD8226 formula `G = 1 + 49.4 kΩ / RG`. Change `RG` on `AMP1.SchDoc` to change the front-end gain.
- **Both filters are Butterworth (Q ≈ 0.7)**. The digital filters in `Softwares/EMGsignal` use the same cutoffs, which sharpens the band edges and removes mains hum.

### Sheets ###

| Sheet               | Function 
|---------------------|----------
| `EMGfrontend.SchDoc`| Top sheet: links the sub-sheets, electrode jack, power/output header, status LED, output RC |
| `VREF.SchDoc`       | Buffered mid-supply reference |
| `AMP1.SchDoc`       | Differential-to-single-ended amplification |
| `HPF(20Hz).SchDoc`  | Removes DC offset and motion artefacts |
| `AMP2.SchDoc`       | Main gain stage |
| `LPF(500Hz).SchDoc` | Anti-aliasing low-pass |

Unless noted otherwise, all passives are 0603 SMD. The full part list with manufacturer numbers is in the ActiveBOM (`BOM_EMGfrontendBomDoc.BomDoc`).

### Simulation (`simulation/`) ###

`Virtual Ground.asc` models the whole chain, from VREF to the ADC input:

- It uses the LTspice `AD8226` model and the generic `UniversalOpAmp2` in place of the MCP6002.
- The input is a 7 mV **chirp** (`B1`) that sweeps from 10 Hz to about 10 kHz over the 0.5 s transient (`.tran 0.5`). Use it to check the pass band.
- The schematic also contains spare test sources: 1.4 mV sines at 125, 250, 500, 1000 and 2000 Hz (`V2`–`V6`) and a 7 mV 200 Hz sine (`V7`). Connect one of them to `SIGNAL` in place of `B1` to test a single frequency.
- Useful probe nodes: `AMPOUT` (after AMP1), `RCHPFOUT` (after HPF), `GAINOUT` (after AMP2), `PROCESSED` (after LPF), `ADC` (board output).

A few values differ slightly from the PCB: the LPF capacitor is 20 nF in the simulation and 22 nF on the board, and the VREF output resistor is 10 Ω in the simulation and 22 Ω on the board.

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
2. Several footprints and symbols come from SnapEDA / Octopart. They are embedded in the documents, so you don't need an external library.
3. Open `simulation/Virtual Ground.asc` in LTspice and press **Run**.
