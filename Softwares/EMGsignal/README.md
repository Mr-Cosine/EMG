# Digital EMG Signal Processing Library #

## Credit: ##

Started from OYMotion's EMGFilters (BSD-2), since fully reimplemented.

Github repo: https://github.com/oymotion/EMGFilters

## Features: ##

A small, header-only C++ library for cleaning up surface EMG (electromyography) signals on microcontrollers in real time. It takes raw ADC samples from an EMG front end and turns them into a rectified, smoothed muscle-activation envelope.

- **Butterworth high-pass and low-pass filters**: designed on the board at startup with the bilinear transform (frequency pre-warped). You choose the order and cutoffs in `constants.h`, and the filter coefficients are computed for you, so there are no hard-coded coefficient tables.
- **Mains hum notch filter**: a cascade of two IIR notches that removes the power-line frequency (50 or 60 Hz) and its 2nd harmonic.
- **Rectification**: the filtered signal is rectified (absolute value) inside the filter chain.
- **Gaussian blur smoothing**: a normalized FIR Gaussian kernel that turns the rectified signal into an envelope. The window length is fixed at compile time.
- **Per-stage enable/bypass**: each stage can be switched on or off, or the whole chain bypassed, with constructor flags.
- **No dynamic memory or STL**: fixed-size ring buffers, fixed-size kernels and a small built-in complex number class, so it also builds on boards that lack the C++ standard library.
- **Portable integer type**: `_int` is 16-bit on AVR and 32-bit everywhere else, so the same headers build on 8-bit and 32-bit boards.
- **Example sketches**:
  - `EMGsignal_ESP.ino`: ESP32, dual-core FreeRTOS version. A hardware timer triggers sampling on core 0, and core 1 is dedicated to processing.
  - `EMGsignal_AVR.ino`: Arduino Uno / Nano / Mega version, sampling from a `TimerOne` interrupt.
- **PC test harness**: runs the same chain on a PC with a generated test signal and plots the result (see [Testing on a PC](#testing-on-a-pc)).

The digital stages match the analog front end in `Electronics/EMG sensor front end` (20 Hz HPF, 500 Hz LPF). The analog filters handle anti-aliasing, and the digital ones sharpen the band edges and remove hum that gets past the hardware.

## Processing Workflow: ##

```
ADC (hardware timer @ sample_freq)
   ↓
High-pass 20 Hz ─► Low-pass 500 Hz ─► Notch hum_freq + 2×hum_freq ─► Rectify |x| ─► Gaussian blur
   └──────────────────────────────── EMGFilters::process()  (emg_filters.h) ─────────────────────┘
   ↓
× adjustment_factor                                                    (sketch)
   ↓
Serial Plotter
```

Each stage except rectification can be turned off with the constructor flags. Rectification always runs unless the whole chain is bypassed. Both example sketches enable every stage, including the blur.

## Quick start (ESP32): ##

1. Install the **esp32** board package (Espressif) **3.x** from the Arduino Boards Manager. The sketch uses the 3.x timer API (`timerBegin(frequency)`, `timerAlarm()`) and doesn't compile on the 2.x core.
2. Set `sample_freq` and `hum_freq` in `src/constants.h` to match your setup (see below).
3. Connect the EMG front-end output to **GPIO33** (ADC1_CH5), or change `SENSOR_INPUT_PIN`. On the [ESP32 breakout board](../../Electronics/README.md#esp32-breakout-board), use the EMG terminal wired to GPIO33.
4. Open `EMGsignal_ESP.ino`, upload it, and open the Serial Plotter at **460800 baud**.
5. The plot shows the envelope (`Signal`) between two fixed reference lines (`Min:0`, `Max:400`) that keep the vertical scale steady.
6. To check the wiring and signal level with raw ADC values, use [`ADCsignalDebug`](../ADCsignalDebug) first. The `ADCdebug` define in the sketch isn't used at the moment.

### How the ESP32 sketch works ###

```
hardware timer ISR (every 1/sample_freq s)
   │  vTaskNotifyGiveFromISR
   ▼
sample task  (core 0, priority 2)  ── analogRead(SENSOR_INPUT_PIN) ──►  adc, new_sample = true
                                                                              │
process task (core 1, priority 1)  ◄──────────────────────────────────────────┘
   EMGFilters::process() × adjustment_factor ─► print every printEvery samples
```

- The ISR only wakes the sample task. `analogRead()` runs in the task, not in the ISR.
- The process task owns core 1 and polls `new_sample` in a loop. Core 0 is left for sampling and, later, motor control.
- `loop()` deletes itself, so all work happens in the two tasks.

## Quick start (AVR): ##

1. Install the **TimerOne** library from the Library Manager.
2. **Set `sample_freq = 2000` in `src/constants.h`.** The sketch samples every 500 µs (2 kHz, `Timer1.initialize(500)`), and the filters are only correct if `sample_freq` matches the real sample rate.
3. Connect the EMG front-end output to `A0`, upload `EMGsignal_AVR.ino`, and open the Serial Plotter at **115200 baud**.

On AVR, all `float` math is done in software, so check that one sample is processed within one sample period (see [Notes](#notes)).

## Minimal use of the filter chain: ##

```cpp
#include "src/emg_filters.h"

#define SENSOR_INPUT_PIN 33 // input pin number

// force_bypass, enable_notch, enable_lopass, enable_hipass, enable_blur
EMGFilters<_float> filter(false, true, true, true, true);

void setup() {
    Serial.begin(460800);   // Depends on your printing rate.
}

void loop() {
    _float raw = (_float)analogRead(SENSOR_INPUT_PIN);
    // must be called exactly once per sample, at sample_freq
    _float envelope = filter.process(raw);   // already rectified
    Serial.println(envelope);
}
```

This minimal loop doesn't keep a fixed sample rate. Use the timer setup from one of the example sketches in real code.

## Configuration: ##

All filters are designed for a fixed sample rate. To configure the sample frequency, the cutoff frequencies and the notch frequency, change the values in `src/constants.h`.

| Constant              | Default   | Meaning |
|-----------------------|-----------|---------|
| `sample_freq`         | 4000      | Sample rate in Hz. Must be more than twice `lpf_cutoff`. The ESP32 sketch's hardware timer period is derived from it. The AVR sketch's period is hard-coded (set this to 2000 for AVR). |
| `filter_order`        | 2         | Butterworth order for both the HPF and LPF. A higher order filters more sharply but takes longer per sample. |
| `hpf_cutoff`          | 20        | High-pass cutoff (Hz). |
| `lpf_cutoff`          | 500       | Low-pass cutoff (Hz). |
| `hum_freq`            | 60        | Power-line frequency: `60` for North America and parts of Asia, `50` for China and most other regions. |
| `quality_factor`      | 30        | Notch Q. Higher means narrower. At 30 the −3 dB width is about 2 Hz around 60 Hz. |
| `GBLUR_SPAN`          | 5         | Half-width of the Gaussian window in samples. The kernel has `2 × GBLUR_SPAN + 1` taps (11 by default). |
| `amplification_factor`| 2.0       | Gain applied after rectification in the PC test harness. The example sketches use their own `adjustment_factor` instead. |

In the sketches:

| Setting             | `EMGsignal_ESP.ino` | `EMGsignal_AVR.ino` | Meaning |
|---------------------|---------------------|---------------------|---------|
| `SENSOR_INPUT_PIN`  | `33`                | `A0`                | ADC input pin |
| Baud rate           | 460800              | 115200              | Serial Plotter speed |
| Sample period       | `1000000 / sample_freq` µs | 500 µs (fixed) | Hardware timer period |
| `adjustment_factor` | 5.0                 | 10.0                | Gain applied to the envelope before printing |
| `printEvery` / `sampleEvery` | 5          | 10                  | Print one value every N samples (800 values/s at 4 kHz on ESP32, 200 values/s at 2 kHz on AVR) |

A cutoff must be strictly between 0 and `sample_freq / 2`. Otherwise that Butterworth stage is left with zero coefficients and outputs 0.
The print data rate must stay within the baud rate.

## API reference: ##

All math is done in `_float` (32-bit `float`). The type aliases are defined in `types.h`:

| Type     | Definition | Notes |
|----------|------------|-------|
| `_float` | `float`    | 32-bit on every supported board |
| `_int`   | `int16_t` on AVR (`ARDUINO_ARCH_AVR`), `int32_t` elsewhere | Native-width integer for indices and counters |
| `_short` | `int8_t`   | Small counters |

### `EMGFilters<dataType>` (`emg_filters.h`) ###
The full filter chain.
- `EMGFilters(bool force_bypass, bool enable_notch, bool enable_lopass, bool enable_hipass, bool enable_blur)` designs all stages. If `force_bypass` is set, or every stage is disabled, the chain is bypassed and returns its input unchanged.
- `dataType process(dataType x)` runs one sample through HPF → LPF → notch → rectify (→ blur, if enabled). Disabled stages are skipped. The output is always ≥ 0 unless the chain is bypassed.
- `lpf()`, `hpf()`, `ahf()`, `gblur()` return references to the individual stages, for example to call a stage directly or to clear its state. Every stage is initialised, even if `process()` skips it.

### `Butterworth`, `Lopass`, `Hipass` (`filter_core.h`) ###
Single Butterworth stages in direct form I.
- `Butterworth::init(_float cutoff, bool is_lpf)` designs a filter with any cutoff.
- `Lopass::init()` / `Hipass::init()` design with `lpf_cutoff` / `hpf_cutoff`.
- `_float process(_float x)`, `void clear_state()`.

### `Notch` (`filter_core.h`) ###
Removes `hum_freq` and `2 × hum_freq` with two biquads in transposed direct form II.
- `void init(_float Q)`: a higher Q gives a narrower notch.
- `_float process(_float x)`, `void clear_state()`.

### `Gaussian_blur<dataType, blur_span>` (`blur.h`) ###
FIR smoothing filter with `2 × blur_span + 1` taps.
- `Gaussian_blur(bool flush = true, dataType filler = 0)`: pre-fills the history with `filler` so the first outputs don't ramp up from garbage.
- `dataType blur(dataType x)` pushes one sample and returns the smoothed value. It is causal, so the output lags by `blur_span` samples.
- `static constexpr _int blur_length()` returns the number of taps.
- σ works out to 1 sample for any `blur_span` (`sigma = (kernel_size − 1) / (2 × blur_span)`), so the window covers ±`blur_span` σ. A larger span adds taps, but the smoothing stays almost the same.

### `Queue<dataType, CAPACITY>` (`queue.h`) ###
Fixed-capacity ring buffer used as the filter history.
- `Queue(bool flush = false, dataType filler = 0)`.
- `push(x)` appends `x`. It does nothing if the buffer is full.
- `pop()` drops the oldest value. It does nothing if the buffer is empty.
- `update(x)` appends `x` and drops the oldest value once the buffer is full.
- `operator[](i)` indexes from oldest (`0`) to newest (`CAPACITY - 1`).
- `flush(filler)`, `size()`, `capacity()`, `isFull()`.

### `Complexf` (`complexf.h`) ###
Lightweight complex number type used to place the filter poles.
- Construct it from (real, imaginary) or from a `Polar_coord`, or with `polar(r, theta)`.
- It supports `+ - * /` with both complex and real operands, compound assignment, `conjugate()`, and `==`, which uses a tolerance of 1e-5.

## Testing on a PC: ##

`test/filter_exported.cpp` runs the filter chain, reading `"<raw> <id>"` lines from stdin and writing `"<envelope> <id>"` to stdout. `test/signal_gen.py` starts it, feeds it a generated signal in real time, and plots the input against the output (needs `matplotlib`).

> **Not yet updated:** `filter_exported.cpp` still uses the old `f32` / `i32` type names, which were renamed to `_float` / `_int`. Rename them before building, or the build fails.

From this folder:

```
mkdir build
cl /EHsc /Fe:build\filter_exported.exe test\filter_exported.cpp     (MSVC)
g++ -O2 -o build/filter_exported.exe test/filter_exported.cpp        (or g++)
python test/signal_gen.py [path/to/filter_exported.exe]
```

Edit `signal()` and the test parameters at the top of `signal_gen.py` to change the test signal. Keep `rate` equal to `sample_freq`.

## Files: ##

| File                     | Contents |
|--------------------------|----------|
| `EMGsignal_ESP.ino`      | ESP32 sketch: timer-driven sampling on core 0, filtering and plotting on core 1 |
| `EMGsignal_AVR.ino`      | AVR sketch: `TimerOne` sampling at 2 kHz, filtering and plotting |
| `src/emg_filters.h`      | `EMGFilters` filter-chain wrapper |
| `src/filter_core.h`      | Butterworth HPF/LPF and the dual notch filter |
| `src/blur.h`             | Gaussian blur envelope smoother |
| `src/queue.h`            | Ring buffer |
| `src/complexf.h`         | Complex number class |
| `src/constants.h`        | Sample rate and filter configuration |
| `src/types.h`            | `_int` / `_short` / `_float` type aliases |
| `src.zip`                | Packaged copy of `src/` and `EMGsignal_ESP.ino` for sharing. It isn't regenerated automatically: the current zip predates the latest changes to `emg_filters.h`, `types.h` and the ESP32 sketch |
| `test/filter_exported.cpp` | PC build of the processing chain (stdin → stdout) |
| `test/signal_gen.py`     | Test signal generator and plotter |

## Notes: ##

- On 8-bit AVR boards (Uno, Nano, Mega), `float` math is done in software. The full chain (two 2nd-order Butterworths, two notches and an 11-tap blur) is fine on ESP32. On AVR, check that one sample finishes within one sample period, and lower `GBLUR_SPAN` or disable the blur if it doesn't.
- Both sketches drop samples silently if processing takes longer than one sample period, because the sampling side only sets a flag. If you add heavy processing or printing, lower `sample_freq` or raise the print interval accordingly.
- The filter headers in `src/` are board-independent. Only the sketches use board-specific timer code (`hw_timer_t` + FreeRTOS on ESP32, `TimerOne` on AVR). To port to another board, such as the STM32G431 on the [EMG integrated sensor](../../Electronics/README.md#emg-integrated-sensor-emgintegrated), replace the timer and ADC setup.
