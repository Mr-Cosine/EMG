#include "Bipolar_stepper_motor.h"
 
double Bipolar_stepper_motor::Speed_levels::interpolate_delay_ms(int speed_level) {
  return (double)this->min_speed.interstep_delay - (double)(this->min_speed.interstep_delay - this->max_speed.interstep_delay)
          /
          (double)(this->max_speed.value - this->min_speed.value)
          *
          ((double)speed_level - 1);
}
 
int Bipolar_stepper_motor::Speed_levels::max_speed_level() { return max_speed.value; }

void Bipolar_stepper_motor::Speed_levels::set_max_speed_level(int level) { max_speed.value = level; }
void Bipolar_stepper_motor::Speed_levels::set_interstep_delay_min_ms(double delay_min) { max_speed.interstep_delay = delay_min; }
void Bipolar_stepper_motor::Speed_levels::set_interstep_delay_max_ms(double delay_max) { min_speed.interstep_delay = delay_max; }
 
// ---- Bipolar_stepper_motor ----
 
void Bipolar_stepper_motor::set_number_of_speed_levels(int number_of_levels) {
  this->speed_levels.set_max_speed_level(number_of_levels);
}
 
void Bipolar_stepper_motor::set_speed_level(int speed_level) {
  int new_speed_level = speed_level;
  if (speed_level < 1) new_speed_level = 1;
  if (speed_level > speed_levels.max_speed_level()) new_speed_level = speed_levels.max_speed_level();
 
  this->current_speed_level = new_speed_level;
}
 
void Bipolar_stepper_motor::set_forward_actuation_length(int actuation_length) { this->actuation_length_forward = actuation_length; }
void Bipolar_stepper_motor::set_backward_actuation_length(int actuation_length) { this->actuation_length_backward = actuation_length; }
void Bipolar_stepper_motor::set_multiplied_actuation_length(double actuation_length_multiplier) {
  this->actuation_length_forward = (int)(this->actuation_length_forward * actuation_length_multiplier);
  this->actuation_length_backward = (int)(this->actuation_length_backward * actuation_length_multiplier);
}
void Bipolar_stepper_motor::set_all_actuation_length_based_on_forward(int actuation_length) {
  double multiplier = (double)actuation_length / this->actuation_length_forward;
  this->actuation_length_forward = (int)ceil(this->actuation_length_forward * multiplier);
  this->actuation_length_backward = (int)ceil(this->actuation_length_backward * multiplier);
}
void Bipolar_stepper_motor::set_all_actuation_length_based_on_backward(int actuation_length) {
  double multiplier = (double)actuation_length / this->actuation_length_backward;
  this->actuation_length_backward = (int)ceil(this->actuation_length_backward * multiplier);
  this->actuation_length_forward = (int)ceil(this->actuation_length_forward * multiplier);
}

void Bipolar_stepper_motor::set_interstep_delay_min_ms(double delay_min) { this->speed_levels.set_interstep_delay_min_ms(delay_min); }
void Bipolar_stepper_motor::set_interstep_delay_max_ms(double delay_max) { this->speed_levels.set_interstep_delay_max_ms(delay_max); }

void Bipolar_stepper_motor::move_forward_single() {
  int stepDelay = (int)(speed_levels.interpolate_delay_ms(current_speed_level) * 1000);
  setStep(HIGH, LOW, HIGH, LOW);
  delayMicroseconds(stepDelay);
  setStep(HIGH, LOW, LOW, HIGH);
  delayMicroseconds(stepDelay);
  setStep(LOW, HIGH, LOW, HIGH);
  delayMicroseconds(stepDelay);
  setStep(LOW, HIGH, HIGH, LOW);
  delayMicroseconds(stepDelay);
}
 
void Bipolar_stepper_motor::move_backward_single() {
  int stepDelay = (int)(speed_levels.interpolate_delay_ms(current_speed_level) * 1000);
  setStep(LOW, HIGH, HIGH, LOW);
  delayMicroseconds(stepDelay);
  setStep(LOW, HIGH, LOW, HIGH);
  delayMicroseconds(stepDelay);
  setStep(HIGH, LOW, LOW, HIGH);
  delayMicroseconds(stepDelay);
  setStep(HIGH, LOW, HIGH, LOW);
  delayMicroseconds(stepDelay);
}
 
void Bipolar_stepper_motor::move_forward() {
  repeat_times(this->actuation_length_forward) { this->move_forward_single(); }
}
 
void Bipolar_stepper_motor::move_backward() {
  repeat_times(this->actuation_length_backward) { this->move_backward_single(); }
}

void Bipolar_stepper_motor::move_forward(int temp_speed) {
  int og_speed = this->current_speed_level;
  this->current_speed_level = temp_speed;
  repeat_times(this->actuation_length_forward) { this->move_forward_single(); }
  this->current_speed_level = og_speed;
}
 
void Bipolar_stepper_motor::move_backward(int temp_speed) {
  int og_speed = this->current_speed_level;
  this->current_speed_level = temp_speed;
  repeat_times(this->actuation_length_backward) { this->move_backward_single(); }
  this->current_speed_level = og_speed;
}
