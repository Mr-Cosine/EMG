### Bipolar Stepper Motor Firmware ###

## Features: ##

Arduino firmware for the linear stepper actuator that drives the gripping motion of the hand exoskeleton. It contains a small driver library for a 4-wire bipolar stepper motor on an H-bridge driver (L298N / DRV8833 style, `IN1`–`IN4`), a button-controlled sketch, and a PC tool for calibrating the motor over serial.

- **Bipolar stepper driver class**: full-step drive with both coils energised, so 4 steps make one actuation cycle. The coils are released after every move, so the motor does not heat up while idle.
- **Speed levels**: speed is given as a level from `1` (slowest) to `N` (fastest). The delay between steps is interpolated linearly between a maximum and a minimum delay, so you set the speed range once and then only choose levels.
- **Actuation length**: the full stroke (bottom to tip) is stored separately for forward and backward as a number of cycles, so `move_forward()` / `move_backward()` run the whole stroke.
- **Temporary speed overrides**: every move function has an overload that runs at a different speed level once and then restores the previous one.
- **Calibration tool**: `motor_setup.py` connects to the board over serial and walks you through pin layout, stroke length and speed range, then saves the result to `motor_config.json`.

## Folder layout: ##

```
StepperMotor/
├── StepperMotor.ino              Main sketch: manual control with 3 buttons
├── src/
│   └── bipolar_stepper_motor.h/.cpp  Motor driver library (master copy)
├── motor_test/
│   ├── test_drive.ino            Runs the full stroke forward and back, forever
│   └── button_test/
│       └── button_test.ino       Prints which button is pressed
└── motor_setup/
    ├── motor_setup.py            PC calibration tool (terminal menu)
    ├── motor_setup.spec          PyInstaller build config
    ├── motor_config.json         Saved calibration result
    └── motor_drive/
        ├── motor_drive.ino       Board-side sketch for motor_setup.py
        └── bipolar_stepper_motor.h/.cpp
```

Arduino only compiles files in the sketch's own folder and its `src/` subfolder, so every sketch that uses the motor needs its own copy of `bipolar_stepper_motor.h` and `bipolar_stepper_motor.cpp`. `motor_drive/` already has one, next to the sketch. Copy them into `motor_test/` before uploading `test_drive.ino`. For `StepperMotor.ino` the library lives in `src/`, so it has to be included as `#include "src/bipolar_stepper_motor.h"`. When you change the library, update the copies in `motor_drive/` as well.

## Wiring: ##

Default pins used by all sketches:

| Signal      | Pin | Meaning |
|-------------|-----|---------|
| `IN1`       | D2  | Coil A, end 1 |
| `IN2`       | D3  | Coil A, end 2 |
| `IN3`       | D4  | Coil B, end 1 |
| `IN4`       | D5  | Coil B, end 2 |
| `btn_fwd`   | D7  | Hold to move forward (`StepperMotor.ino` only) |
| `btn_bwd`   | D8  | Hold to move backward (`StepperMotor.ino` only) |
| `btn_spd`   | D9  | Press to go to the next speed level (`StepperMotor.ino` only) |

The buttons use the internal pull-ups. Wire each one between its pin and GND, with no external resistor.

If the motor vibrates instead of turning, one coil is reversed. Swap `IN1` and `IN2` (or `IN3` and `IN4`).

## Quick start: ##

### Manual control (`StepperMotor.ino`) ###

1. Wire the driver and the three buttons as above.
2. Open `StepperMotor.ino`, upload it, and open the Serial Monitor at **9600 baud**.
3. Hold the forward or backward button to jog the motor. Each loop runs one cycle (4 steps), and the Serial Monitor prints the direction with a repeat counter.
4. Press the speed button to go up one speed level. After the highest level it wraps back to level 1. The sketch uses 20 levels and starts at level 1.

Pressing more than one button at the same time does nothing.

### Calibration (`motor_setup/`) ###

1. Upload `motor_setup/motor_drive/motor_drive.ino` to the board. It prints `READY` on boot and then waits for commands.
2. On the PC, install the dependencies: `pyserial` and `keyboard`, plus the `prettier_console` package, which must be placed in the `motor_setup/` folder (it is not part of this repo).
3. Run `python motor_setup.py` from inside `motor_setup/`, or run the built `dist/motor_setup/motor_setup.exe`.
4. In the menu:
   - **set serial port**: pick the Arduino's COM port. Do this first, because the other steps need the connection. Bluetooth COM ports are not the Arduino.
   - **setup for pin layout**: assign `IN1`–`IN4` to digital pins D2–D13.
   - **caliberate actuation length**: move the slider to the bottom, then count how many jogs it takes to reach the tip and come back. Each jog is 10 cycles, so the result is a multiple of 10.
   - **adjust interstep delays**: enter the minimum delay (fastest speed) and maximum delay (slowest speed) in ms. You can test-drive each value before keeping it.
   - **quit and save**: writes `motor_config.json`. This is the only option that saves anything.
5. Copy the values from `motor_config.json` into your sketch's `setup()` (see the example below). The board does not store the calibration.

To rebuild the executable: `pyinstaller motor_setup.spec`.

### Serial protocol (`motor_drive.ino`) ###

One command per line, newline-terminated, at 9600 baud. Arguments come after a comma. The board replies `OK`, or `ERR <reason>`.

| Command     | Action |
|-------------|--------|
| `FWD`       | Move forward 10 cycles |
| `BWD`       | Move backward 10 cycles |
| `DMIN,<ms>` | Set the minimum interstep delay (fastest speed) |
| `DMAX,<ms>` | Set the maximum interstep delay (slowest speed) |
| `PIN1,<n>` … `PIN4,<n>` | Change the pin used for `IN1` … `IN4` |

`motor_drive.ino` runs at its top speed level (20 of 20), so `FWD`/`BWD` move at the minimum delay.

## Using the library: ##

```cpp
#include "bipolar_stepper_motor.h"

Bipolar_stepper_motor motor(2, 3, 4, 5);   // IN1, IN2, IN3, IN4

void setup() {
  // values from motor_config.json
  motor.set_number_of_speed_levels(20);
  motor.set_interstep_delay_min_ms(2);      // delay at the fastest level
  motor.set_interstep_delay_max_ms(20);     // delay at the slowest level
  motor.set_forward_actuation_length(370);  // cycles from bottom to tip
  motor.set_backward_actuation_length(370); // cycles from tip to bottom
  motor.set_speed_level(20);
}

void loop() {
  motor.move_forward();       // full stroke at the current speed level
  delay(1000);
  motor.move_backward(10);    // full stroke at level 10, then back to level 20
  delay(1000);
}
```

## Speed model: ##

The delay between steps at speed level `L` (out of `N` levels) is

```
delay(L) = delay_max - (delay_max - delay_min) / (N - 1) * (L - 1)
```

so level `1` uses `delay_max` and level `N` uses `delay_min`. One cycle takes `4 × delay(L)`, so a full stroke takes about `4 × actuation_length × delay(L)`. For example, 370 cycles at 2 ms per step take about 3 s.

The default is 10 levels, 5 ms minimum delay and 10 ms maximum delay.

## API reference: ##

### `Bipolar_stepper_motor` (`bipolar_stepper_motor.h`) ###

**Constructor**
- `Bipolar_stepper_motor(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4)` stores the pins and sets them to `OUTPUT`. Actuation lengths start at 0 and the speed level at 1.

**Speed**
- `void set_number_of_speed_levels(level_t n)` sets how many levels there are. Values below 2 are ignored. The current level is clamped to the new range.
- `void set_speed_level(level_t level)` sets the current level, clamped to `1 … n`.
- `void set_interstep_delay_min_ms(float ms)` sets the delay at the fastest level.
- `void set_interstep_delay_max_ms(float ms)` sets the delay at the slowest level.
- `level_t get_number_of_speed_levels()`, `level_t get_current_speed_level()`.

**Actuation length** (in cycles of 4 steps)
- `void set_forward_actuation_length(length_t)`, `void set_backward_actuation_length(length_t)`.
- `void set_multiplied_actuation_length(float k)` scales both lengths by `k`.
- `void set_all_actuation_length_based_on_forward(length_t)` sets the forward length and scales the backward length by the same ratio. `..._based_on_backward` does the same the other way round.
- `length_t get_forward_actuation_length()`, `length_t get_backward_actuation_length()`.

**Pins**
- `void set_pin_IN1(uint8_t)` … `void set_pin_IN4(uint8_t)` change a pin after construction.

**Movement** (all blocking, coils released afterwards)
- `void move_forward_single()`, `void move_backward_single()` run one cycle (4 steps).
- `void move_forward()`, `void move_backward()` run the full actuation length.
- Each of these has a `(level_t temp_speed)` overload that runs at `temp_speed` and then restores the previous level.

### Helper macros ###
The header also defines some loop macros used by the sketches: `repeat_times(n)`, `repeat_times_w_counter(i, n)`, `repeat_until(cond)`, `repeat_while(cond)` and `repeat_forever`.

## Notes: ##

- All moves are blocking (`delayMicroseconds`), so `loop()` cannot do anything else, such as reading the EMG sensor, while the motor is running.
- The step delay is stored in microseconds as a `uint16_t`, so delays above about 65 ms per step overflow. Keep `delay_max` below 65 ms.
- `set_pin_IN1` … `set_pin_IN4` do not call `pinMode`. A new pin set this way (including through the `PIN1`–`PIN4` serial commands) must be set to `OUTPUT` separately, or the motor will not move.
- `set_all_actuation_length_based_on_forward/backward` divide by the current length. Set a non-zero forward and backward length first.
- `motor_setup.py` saves the calibration on the PC only. The board starts from the values written in the sketch every time it boots.
