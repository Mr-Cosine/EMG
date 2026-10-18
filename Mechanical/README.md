# Mechanical #

## Overview: ##

SolidWorks CAD for the hand exoskeleton. It has three parts: a handpiece on the back of the hand, finger mechanisms driven by a glider sliding on a rail, and an armband base for the forearm. The glider is moved by the linear stepper actuator (firmware in [`Softwares/StepperMotor`](../Softwares/StepperMotor)) to close and open the hand.

This part of the project is currently developed as a personal project.

## Folder layout: ##

```
Mechanical/
├── new hand - metal glider v5.SLDASM   Top-level assembly (open this)
├── handpiece v5.SLDPRT                 Handpiece used in the v5 assembly
├── handpiece v6.SLDPRT                 Newest handpiece (work in progress)
├── armband_base.SLDPRT                 Forearm armband base
├── Fits.txt                            Fit clearances (SolidWorks equations)
├── finger/                             Finger mechanism parts and assemblies
├── CAD drawing/                        Manufacturing drawings and exports
└── legacy files/                       Earlier design iterations
      ├── v1
      ├── v2
      ├── v3
      ├── v4
      └── v5
```

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

### `CAD drawing/` ###

| File | Description |
|------|-------------|
| `glider base_4_CNC_easy.SLDDRW` | 2D drawing of the CNC glider base |

## Fits: ##

`Fits.txt` holds the clearances shared across parts, in SolidWorks equation format:

| Variable | Value   | Meaning |
|----------|---------|---------|
| `LF`     | 0.04 mm | Loose fit (parts that must move freely) |
| `TF`     | 0.02 mm | Tight fit (parts that must hold in place) |
| `AngLF`  | 0.5 deg | Angular loose fit (unit not given in the file) |
| `AngTF`  | 0.3 deg | Angular tight fit (unit not given in the file) |

To use them in a part, go to **Tools › Equations › Link to external file** and select `Fits.txt`. Then use `"LF"`, `"TF"` and so on in dimensions. If you change a value in the file, every linked part updates on rebuild.

## Design history (`legacy files/`): ##

| Version | Assembly | Main change |
|---------|----------|-------------|
| v1 | `hand.SLDASM` | First hand: fixed positoon for each finger. Flexibility comes from beam's elasticity. Includes the thumb |
| v2 | `new hand - normal glider.SLDASM` | Change to sliding finger base for comfort in movement. Square shaped 3D printed glider and slot |
| v3 | `new hand - circular glider.SLDASM` | Circular profile 3D printed glider |
| v4 | `new hand - metal glider (v4).SLDASM` | Changed glider to metal glider and rail |
| v5 | `new hand - metal glider v5.SLDASM` | Handpiece splited to upper and lower part; The active design for now(v6 is still under developping) |

`legacy files/armpiece.SLDPRT` is an earlier forearm part, replaced by `armband_base.SLDPRT`.

Older versions are kept for reference only. Don't edit them. To start a new version, copy the current files into `legacy files/vN/` first, then work on the top-level files.

## Notes: ##

- Open the `.SLDASM` files from inside their own folder structure. SolidWorks resolves parts by relative path, so moving or renaming a part breaks the assembly references.
- If some file prompts missing `Fits.txt`, open browser and select `Fits.txt` in this folder to replace the missing references.
