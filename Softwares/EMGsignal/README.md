### Digital EMG Signal Processing Library ###

## Credit: ##

Started from OYMotion's EMGFilters (BSD-2), since fully reimplemented.

Github repo: https://github.com/oymotion/EMGFilters

## Features: ##

A small, header-only C++ library for cleaning up surface EMG (electromyography) signals on Arduino-class microcontrollers in real time. It takes raw ADC samples from an EMG front end and turns them into a smooth muscle-activation envelope.

- **Butterworth high-pass and low-pass filters**: designed on the board at startup with the bilinear transform (frequency pre-warped). You choose the order and cutoffs in `constants.h`, and the filter coefficients are computed for you, so there are no hard-coded coefficient tables.
- **Mains hum notch filter**: a cascade of two IIR notches that removes the power-line frequency (50 or 60 Hz) and its 2nd harmonic.
- **Gaussian blur smoothing**: a normalized FIR Gaussian kernel for turning the rectified signal into an envelope. The window length is fixed at compile time.
- **Per-stage enable/bypass**: each filter stage can be switched on or off, or the whole chain bypassed, without changing code.
- **No dynamic containers or STL**: fixed-size ring buffers and a small built-in complex number class, so it builds on AVR boards that lack the C++ standard library.
- **Example sketch**: timer-driven sampling at a fixed rate, with output ready for the Arduino Serial Plotter.

The digital stages match the analog front end in `Electronics/EMG sensor front end` (20 Hz HPF, 500 Hz LPF). The analog filters handle anti-aliasing, and the digital ones sharpen the band edges and remove hum that gets past the hardware.

## Processing Workflow: ##

```
ADC (ESP32 hardware timer @ sample_freq)
   ↓
High-pass 20 Hz ─► Low-pass 500 Hz ─► Notch hum_freq + 2×hum_freq      (EMGFilters.h)
   ↓
Rectify(absolute value) × amplification_factor (arduino sketch)
   ↓
Gaussian blur (arduino sketch & blur.h)
   ↓
Output
```

## Quick start: ##

1. Install the **esp32** board package (Espressif) from the Arduino Boards Manager. The example sketch uses the ESP32 hardware timer for fixed-rate sampling, and works with both the 2.x and 3.x cores.
2. Set `sample_freq` and `hum_freq` in `constants.h` to match your setup (see below).
3. Connect the EMG front-end output to `A0` (or change `SENSOR_INPUT_PIN`).
4. Open `EMGsignal.ino`, upload it, and open the Serial Plotter at **115200 baud**.
5. Set `ADCdebug` to `true` to plot the raw ADC values, which is useful for checking wiring and signal levels. Set it to `false` to plot the filtered and blurred envelope.

Minimal use of the filter chain on its own:

```cpp
#include "EMGFilters.h"
#include "Arduino.h"

#define SENSOR_INPUT_PIN A0 // input pin number

EMGFilters<double> filter;

void setup() {
    // bypass_all, enable_notch, enable_lopass, enable_hipass
    filter.init(false, true, true, true);

    Serial.begin(115200);   // Depends on your printing rate.
}

void loop() {
    raw = analogRead(SENSOR_INPUT_PIN);
    // must be called exactly once per sample, at sample_freq
    double filtered = filter.process(raw);
    Serial.println(filtered);
}
```

## Configuration: ##

All filters are designed for a fixed sample rate. Configurations for sample frequency, cutoff frequencies, notch frequency are done by altering the values in constants.h.

| Constant              | Default   | Meaning |
|-----------------------|-----------|---------|
| `sample_freq`         | 2000      | Sample rate in Hz. Must be more than twice `lpf_cutoff`. The sketch's hardware timer period is derived from it. |
| `filter_order`        | 2         | Butterworth order for both the HPF and LPF. The higher the better the filtering, but costs more time. |
| `hpf_cutoff`          | 20        | High-pass cutoff (Hz). |
| `lpf_cutoff`          | 500       | Low-pass cutoff (Hz). |
| `hum_freq`            | 60        | Power-line frequency: `60` for North America and parts of Asia, `50` for China and most other regions. |
| `GBLUR_SPAN`          | 3.0       | How many sigmas the Gaussian window spans on one side (3 means ±3σ). |
| `amplification_factor`| 2.0       | Gain applied after rectification. |

A cutoff must be strictly between 0 and `sample_freq / 2`. Otherwise that Butterworth stage is left with zero coefficients and outputs 0.
The print data rate must stay within baud rate. 115200 is recommended.

## API reference: ##

### `EMGFilters<dataType>` (`EMGFilters.h`) ###
The full filter chain.
- `void init(bool bypass_all, bool enable_notch, bool enable_lopass, bool enable_hipass)` designs all filters and resets their state. The notch Q is 30, giving a −3 dB width of about 2 Hz around 60 Hz.
- `dataType process(dataType x)` runs one sample through HPF → LPF → notch. The math is done internally in `double`.

### `Butterworth`, `Lopass`, `Hipass` (`filter_core.h`) ###
Single Butterworth stages in direct form I.
- `Butterworth::init(double cutoff, bool is_lpf)` designs a filter with any cutoff.
- `Lopass::init()` / `Hipass::init()` design with `lpf_cutoff` / `hpf_cutoff`.
- `double process(double x)`, `void clear_state()`.

### `Notch` (`filter_core.h`) ###
Removes `hum_freq` and `2 × hum_freq` with two biquads in transposed direct form II.
- `void init(double Q)`: a higher Q gives a narrower notch.
- `double process(double x)`, `void clear_state()`.

### `Gaussian_blur<dataType, kernel_size>` (`blur.h`) ###
FIR smoothing filter.
- `Gaussian_blur(bool flush = true, dataType filler = 0)`: pre-fills the history with `filler` so the first outputs don't ramp up from garbage.
- `dataType blur(dataType x)` pushes one sample and returns the smoothed value. It is causal, so the output lags by about `(kernel_size - 1) / 2` samples.
- `static constexpr int blur_length()`.
- The kernel is allocated on the heap once, at construction. Copying is disabled.

### `Queue<dataType, CAPACITY>` (`queue.h`) ###
Fixed-capacity ring buffer used as the filter history.
- `update(x)` appends `x` and drops the oldest value once the buffer is full.
- `operator[](i)` indexes from oldest (`0`) to newest (`CAPACITY - 1`).
- `flush(filler)`, `size()`, `capacity()`, `isFull()`.

### `Complexf` (`complexf.h`) ###
Lightweight complex number type used to place the filter poles.
- Construct it from (real, imaginary) or from a `Polar_coord`, or with `polar(r, theta)`.
- It supports `+ - * /` with both complex and real operands, compound assignment, `conjugate()`, and `==`, which uses a tolerance of 1e-5.

## Files: ##

| File            | Contents |
|-----------------|----------|
| `EMGsignal.ino` | Example sketch: sampling, filtering, rectification, smoothing, plotting |
| `EMGFilters.h`  | `EMGFilters` filter-chain wrapper |
| `filter_core.h` | Butterworth HPF/LPF and the dual notch filter |
| `blur.h`        | Gaussian blur envelope smoother |
| `queue.h`       | Ring buffer |
| `complexf.h`    | Complex number class |
| `constants.h`   | Sample rate and filter configuration |

## Notes: ##

- On 8-bit AVR boards (Uno, Nano, Mega), `double` is the same 32-bit type as `float`, so precision is single-precision there. This is fine at the default 2nd order. At higher orders or very low cutoff/sample-rate ratios, check the response.
- The sketch drops samples silently if `loop()` takes longer than one sample period, because the ISR only sets a flag. If you add heavy processing or printing, lower `sample_freq` or `print_every` accordingly.
- The example sketch targets ESP32 (`hw_timer_t`, `IRAM_ATTR`). On other boards, replace the timer setup with that board's hardware timer (e.g. `TimerOne` on AVR). The filter headers in `src/` themselves are board-independent.
