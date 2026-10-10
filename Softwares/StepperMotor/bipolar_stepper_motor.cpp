#include "bipolar_stepper_motor.h"
 
float Bipolar_stepper_motor::Speed_levels::interpolate_delay_ms(level_t speed_level) {
  return (float)this->min_speed.interstep_delay - (float)(this->min_speed.interstep_delay - this->max_speed.interstep_delay)
          /
          (float)(this->max_speed.value - this->min_speed.value)
          *
          ((float)speed_level - 1);
}

level_t Bipolar_stepper_motor::Speed_levels::max_speed_level() { return max_speed.value; }

void Bipolar_stepper_motor::Speed_levels::set_max_speed_level(level_t level) { max_speed.value = level; }
void Bipolar_stepper_motor::Speed_levels::set_interstep_delay_min_ms(float delay_min) { max_speed.interstep_delay = delay_min; }
void Bipolar_stepper_motor::Speed_levels::set_interstep_delay_max_ms(float delay_max) { min_speed.interstep_delay = delay_max; }
 
// ---- Bipolar_stepper_motor ----
 
void Bipolar_stepper_motor::set_number_of_speed_levels(level_t number_of_levels) {
  if (number_of_levels < 2) return;
  this->speed_levels.set_max_speed_level(number_of_levels);
  this->set_speed_level(this->current_speed_level);
}
 
void Bipolar_stepper_motor::set_speed_level(level_t speed_level) {
  level_t new_speed_level = speed_level;
  if (speed_level < 1) new_speed_level = 1;
  if (speed_level > speed_levels.max_speed_level()) new_speed_level = speed_levels.max_speed_level();
 
  this->current_speed_level = new_speed_level;
}
 
void Bipolar_stepper_motor::set_forward_actuation_length(length_t actuation_length) { this->actuation_length_forward = actuation_length; }
void Bipolar_stepper_motor::set_backward_actuation_length(length_t actuation_length) { this->actuation_length_backward = actuation_length; }
void Bipolar_stepper_motor::set_multiplied_actuation_length(float actuation_length_multiplier) {
  this->actuation_length_forward = (length_t)(this->actuation_length_forward * actuation_length_multiplier);
  this->actuation_length_backward = (length_t)(this->actuation_length_backward * actuation_length_multiplier);
}
void Bipolar_stepper_motor::set_all_actuation_length_based_on_forward(length_t actuation_length) {
  float multiplier = (float)actuation_length / this->actuation_length_forward;
  this->actuation_length_forward = (length_t)(this->actuation_length_forward * multiplier);
  this->actuation_length_backward = (length_t)(this->actuation_length_backward * multiplier);
}
void Bipolar_stepper_motor::set_all_actuation_length_based_on_backward(length_t actuation_length) {
  float multiplier = (float)actuation_length / this->actuation_length_backward;
  this->actuation_length_backward = (length_t)(this->actuation_length_backward * multiplier);
  this->actuation_length_forward = (length_t)(this->actuation_length_forward * multiplier);
}

void Bipolar_stepper_motor::set_interstep_delay_min_ms(float delay_min) { this->speed_levels.set_interstep_delay_min_ms(delay_min); }
void Bipolar_stepper_motor::set_interstep_delay_max_ms(float delay_max) { this->speed_levels.set_interstep_delay_max_ms(delay_max); }

void Bipolar_stepper_motor::cycle_forward(length_t stepDelay) {
  setStep(HIGH, LOW, HIGH, LOW);
  delayMicroseconds(stepDelay);
  setStep(HIGH, LOW, LOW, HIGH);
  delayMicroseconds(stepDelay);
  setStep(LOW, HIGH, LOW, HIGH);
  delayMicroseconds(stepDelay);
  setStep(LOW, HIGH, HIGH, LOW);
  delayMicroseconds(stepDelay);
}
 
void Bipolar_stepper_motor::cycle_backward(length_t stepDelay) {
  setStep(LOW, HIGH, HIGH, LOW);
  delayMicroseconds(stepDelay);
  setStep(LOW, HIGH, LOW, HIGH);
  delayMicroseconds(stepDelay);
  setStep(HIGH, LOW, LOW, HIGH);
  delayMicroseconds(stepDelay);
  setStep(HIGH, LOW, HIGH, LOW);
  delayMicroseconds(stepDelay);
}

void Bipolar_stepper_motor::move_forward_single() {
  length_t stepDelay = (length_t)(speed_levels.interpolate_delay_ms(current_speed_level) * 1000);
  this->cycle_forward(stepDelay);
  this->resetStep();
}

void Bipolar_stepper_motor::move_backward_single() {
  length_t stepDelay = (length_t)(speed_levels.interpolate_delay_ms(current_speed_level) * 1000);
  this->cycle_backward(stepDelay);
  this->resetStep();
}

void Bipolar_stepper_motor::move_forward_single(level_t temp_speed) {
  level_t og_speed = this->current_speed_level;
  this->set_speed_level(temp_speed);
  this->move_forward_single();
  this->set_speed_level(og_speed);
}

void Bipolar_stepper_motor::move_backward_single(level_t temp_speed) {
  level_t og_speed = this->current_speed_level;
  this->set_speed_level(temp_speed);
  this->move_backward_single();
  this->set_speed_level(og_speed);
}
 
void Bipolar_stepper_motor::move_forward() {
  length_t stepDelay = (length_t)(speed_levels.interpolate_delay_ms(current_speed_level) * 1000);
  repeat_times(this->actuation_length_forward) { this->cycle_forward(stepDelay); }
  this->resetStep();
}

void Bipolar_stepper_motor::move_backward() {
  length_t stepDelay = (length_t)(speed_levels.interpolate_delay_ms(current_speed_level) * 1000);
  repeat_times(this->actuation_length_backward) { this->cycle_backward(stepDelay); }
  this->resetStep();
}

void Bipolar_stepper_motor::move_forward(level_t temp_speed) {
  level_t og_speed = this->current_speed_level;
  this->set_speed_level(temp_speed);
  length_t stepDelay = (length_t)(speed_levels.interpolate_delay_ms(current_speed_level) * 1000);
  repeat_times(this->actuation_length_forward) { this->cycle_forward(stepDelay); }
  this->resetStep();
  this->set_speed_level(og_speed);
}

void Bipolar_stepper_motor::move_backward(level_t temp_speed) {
  level_t og_speed = this->current_speed_level;
  this->set_speed_level(temp_speed);
  length_t stepDelay = (length_t)(speed_levels.interpolate_delay_ms(current_speed_level) * 1000);
  repeat_times(this->actuation_length_backward) { this->cycle_backward(stepDelay); }
  this->resetStep();
  this->set_speed_level(og_speed);
}
