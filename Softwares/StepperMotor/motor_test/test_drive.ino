#include "bipolar_stepper_motor.h"

const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;

Bipolar_stepper_motor motor(IN1, IN2, IN3, IN4);

// optimized setting for the motor specifically. need to test out.
void setup() {
  motor.set_number_of_speed_levels(20);
  motor.set_interstep_delay_min_ms(2.0);
  motor.set_forward_actuation_length(370);
  motor.set_backward_actuation_length(370);
  motor.set_speed_level(20);

  Serial.begin(9600);
}


void loop() {
  motor.move_forward();
  delay(1000);
  motor.move_backward();
  delay(1000);
}
