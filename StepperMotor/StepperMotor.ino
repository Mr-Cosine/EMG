#include "bipolar_stepper_motor.h"

const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;

Bipolar_stepper_motor motor(IN1, IN2, IN3, IN4);

void setup() {
  motor.set_number_of_speed_levels(10);
  motor.set_speed_level(1);
  motor.set_forward_actuation_length(100);
  motor.set_backward_actuation_length(110);
}

void loop() {
  int speed_level = 1;
  repeat_times(5) {
    motor.set_speed_level(speed_level);
    motor.move_forward();
    delay(500);
    motor.move_backward();
    delay(500);
    speed_level++;
  }

  motor.set_multiplied_actuation_length(0.2);
  speed_level = 1;
  repeat_times(5) {
    motor.set_speed_level(speed_level);
    motor.move_forward();
    delay(500);
    motor.move_backward();
    delay(500);
    speed_level++;
  }
}
