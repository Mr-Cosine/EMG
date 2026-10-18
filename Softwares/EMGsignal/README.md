# Digital EMG Signal Processing Library #

## Credit: ##

Started from OYMotion's EMGFilters (BSD-2), since fully reimplemented.

Github repo: https://github.com/oymotion/EMGFilters

## Features: ##

A small, header-only C++ library for cleaning up surface EMG (electromyography) signals on microcontrollers in real time. It takes raw ADC samples from an EMG front end and turns them into a smooth muscle-activation envelope.

- **Butterworth high-pass and low-pass filters**: designed on the board at startup with the bilinear transform (frequency pre-warped). You choose the order and cutoffs in `constants.h`, and the filter coefficients are computed for you, so there are no hard-coded coefficient tables.
- **Mains hum notch filter**: a cascade of two IIR notches that removes the power-line frequency (50 or 60 Hz) and its 2nd harmonic.
- **Gaussian blur smoothing**: a normalized FIR Gaussian kernel for turning the rectified signal into an envelope. The window length is fixed at compile time.
- **Per-stage enable/bypass**: each stage can be switched on or off, or the whole chain bypassed, with constructor flags.
- **No dynamic memory or STL**: fixed-size ring buffers, fixed-size kernels and a small built-in complex number class, so it also builds on boards that lack the C++ standard library.
- **Example sketch**: timer-driven sampling at a fixed rate on ESP32, with output ready for the Arduino Serial Plotter.
- **PC test harness**: runs the same chain on a PC with a generated test signal and plots the result (see [Testing on a PC](#testing-on-a-pc-)).

The digital stages match the analog front end in `Electronics/EMG sensor front end` (20 Hz HPF, 500 Hz LPF). The analog filters handle anti-aliasing, and the digital ones sharpen the band edges and remove hum that gets past the hardware.

## Processing Workflow: ##

```
ADC (ESP32 hardware timer @ sample_freq)
   ↓
High-pass 20 Hz ─► Low-pass 500 Hz ─► Notch hum_freq + 2×hum_freq      (emg_filters.h)
   ↓
Rectify (absolute value) × amplification_factor − floor_signal, clamped at 0   (arduino sketch)
   ↓
Gaussian blur                                                          (blur.h, via filter.gblur())
   ↓
Output
```

`EMGFilters` can also run the blur as a last stage inside `process()` (5th constructor flag). The example sketch leaves that off and calls `filter.gblur().blur()` itself, so the blur runs on the rectified signal.

## Quick start: ##

1. Install the **esp32** board package (Espressif) from the Arduino Boards Manager. The example sketch uses the ESP32 hardware timer for fixed-rate sampling, and works with both the 2.x and 3.x cores.
2. Set `sample_freq` and `hum_freq` in `src/constants.h` to match your setup (see below).
3. Connect the EMG front-end output to `A0` (GPIO36 / ADC1_CH0 on ESP32 dev boards), or change `SENSOR_INPUT_PIN`.
4. Open `EMGsignal.ino`, upload it, and open the Serial Plotter at **115200 baud**.
5. Set `ADCdebug` to `true` to plot the raw ADC values, which is useful for checking wiring and signal levels. Set it to `false` to plot the filtered and blurred envelope.
6. Calibrate the noise floor: put on the electrodes, relax the muscle, and note the largest envelope value over a few seconds. Write it into `floor_signal`. Anything below it is set to zero.

Minimal use of the filter chain on its own:

```cpp
#include "src/emg_filters.h"

#define SENSOR_INPUT_PIN A0 // input pin number

// bypass_all, enable_notch, enable_lopass, enable_hipass, enable_blur
EMGFilters<f32> filter(false, true, true, true, false);

void setup() {
    Serial.begin(115200);   // Depends on your printing rate.
}

void loop() {
    f32 raw = (f32)analogRead(SENSOR_INPUT_PIN);
    // must be called exactly once per sample, at sample_freq
    f32 filtered = filter.process(raw);
    Serial.println(filtered);
}
```

## Configuration: ##

All filters are designed for a fixed sample rate. To configure the sample frequency, the cutoff frequencies and the notch frequency, change the values in `src/constants.h`.

| Constant              | Default   | Meaning |
|-----------------------|-----------|---------|
| `sample_freq`         | 2000      | Sample rate in Hz. Must be more than twice `lpf_cutoff`. The sketch's hardware timer period is derived from it. |
| `filter_order`        | 2         | Butterworth order for both the HPF and LPF. A higher order filters more sharply but takes longer per sample. |
| `hpf_cutoff`          | 20        | High-pass cutoff (Hz). |
| `lpf_cutoff`          | 500       | Low-pass cutoff (Hz). |
| `hum_freq`            | 60        | Power-line frequency: `60` for North America and parts of Asia, `50` for China and most other regions. |
| `quality_factor`      | 30        | Notch Q. Higher means narrower. At 30 the −3 dB width is about 2 Hz around 60 Hz. |
| `GBLUR_SPAN`          | 3         | Half-width of the Gaussian window in samples. The kernel has `2 × GBLUR_SPAN + 1` taps and spans ±3σ. |
| `amplification_factor`| 2.0       | Gain applied after rectification. |

In the sketch itself:

| Setting         | Default | Meaning |
|-----------------|---------|---------|
| `ADCdebug`      | `true`  | `true` plots the raw ADC value, `false` plots the envelope. |
| `floor_signal`  | 0       | Noise floor subtracted from the envelope (see Quick start step 6). |
| `print_every`   | 10      | Print one value every N samples (200 values/s at 2 kHz). |

A cutoff must be strictly between 0 and `sample_freq / 2`. Otherwise that Butterworth stage is left with zero coefficients and outputs 0.
The print data rate must stay within the baud rate. 115200 is recommended.

## API reference: ##

All math is done in `f32` (32-bit `float`), defined with `i32` in `types.h`.

### `EMGFilters<dataType>` (`emg_filters.h`) ###
The full filter chain.
- `EMGFilters(bool bypass_all, bool enable_notch, bool enable_lopass, bool enable_hipass, bool enable_blur)` designs all stages. If every stage is disabled, the chain is bypassed.
- `dataType process(dataType x)` runs one sample through HPF → LPF → notch (→ blur, if enabled).
- `lpf()`, `hpf()`, `ahf()`, `gblur()` return references to the individual stages, for example to call a stage directly or to clear its state.

### `Butterworth`, `Lopass`, `Hipass` (`filter_core.h`) ###
Single Butterworth stages in direct form I.
- `Butterworth::init(f32 cutoff, bool is_lpf)` designs a filter with any cutoff.
- `Lopass::init()` / `Hipass::init()` design with `lpf_cutoff` / `hpf_cutoff`.
- `f32 process(f32 x)`, `void clear_state()`.

### `Notch` (`filter_core.h`) ###
Removes `hum_freq` and `2 × hum_freq` with two biquads in transposed direct form II.
- `void init(f32 Q)`: a higher Q gives a narrower notch.
- `f32 process(f32 x)`, `void clear_state()`.

### `Gaussian_blur<dataType, blur_span>` (`blur.h`) ###
FIR smoothing filter with `2 × blur_span + 1` taps.
- `Gaussian_blur(bool flush = true, dataType filler = 0)`: pre-fills the history with `filler` so the first outputs don't ramp up from garbage.
- `dataType blur(dataType x)` pushes one sample and returns the smoothed value. It is causal, so the output lags by `blur_span` samples.
- `static constexpr i32 blur_length()` returns the number of taps.

### `Queue<dataType, CAPACITY>` (`queue.h`) ###
Fixed-capacity ring buffer used as the filter history.
- `Queue(bool flush = false, dataType filler = 0)`.
- `update(x)` appends `x` and drops the oldest value once the buffer is full.
- `operator[](i)` indexes from oldest (`0`) to newest (`CAPACITY - 1`).
- `flush(filler)`, `size()`, `capacity()`, `isFull()`.

### `Complexf` (`complexf.h`) ###
Lightweight complex number type used to place the filter poles.
- Construct it from (real, imaginary) or from a `Polar_coord`, or with `polar(r, theta)`.
- It supports `+ - * /` with both complex and real operands, compound assignment, `conjugate()`, and `==`, which uses a tolerance of 1e-5.

## Testing on a PC: ##

`test/filter_exported.cpp` runs the same chain as the sketch, reading `"<raw> <id>"` lines from stdin and writing `"<envelope> <id>"` to stdout. `test/signal_gen.py` starts it, feeds it a generated signal in real time, and plots the input against the output (needs `matplotlib`).

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
| `EMGsignal.ino`          | Example sketch: sampling, filtering, rectification, smoothing, plotting |
| `src/emg_filters.h`      | `EMGFilters` filter-chain wrapper |
| `src/filter_core.h`      | Butterworth HPF/LPF and the dual notch filter |
| `src/blur.h`             | Gaussian blur envelope smoother |
| `src/queue.h`            | Ring buffer |
| `src/complexf.h`         | Complex number class |
| `src/constants.h`        | Sample rate and filter configuration |
| `src/types.h`            | `i32` / `f32` type aliases |
| `test/filter_exported.cpp` | PC build of the processing chain (stdin → stdout) |
| `test/signal_gen.py`     | Test signal generator and plotter |

## Notes: ##

- On 8-bit AVR boards (Uno, Nano, Mega), `float` math is done in software. The default 2nd-order chain at 2 kHz is fine on ESP32. On AVR, check that one sample finishes within one sample period.
- The sketch drops samples silently if `loop()` takes longer than one sample period, because the ISR only sets a flag. If you add heavy processing or printing, lower `sample_freq` or raise `print_every` accordingly.
- The example sketch targets ESP32 (`hw_timer_t`, `IRAM_ATTR`). On other boards, replace the timer setup with that board's hardware timer (e.g. `TimerOne` on AVR). The filter headers in `src/` themselves are board-independent.
