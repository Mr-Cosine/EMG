#include "bipolar_stepper_motor.h"

const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;

const int btn_fwd = 7;
const int btn_bwd = 8;
const int btn_spd = 9;

Bipolar_stepper_motor motor(IN1, IN2, IN3, IN4);

void setup() {
  motor.set_number_of_speed_levels(20);
  motor.set_interstep_delay_min_ms(3);
  motor.set_speed_level(1);

  pinMode(btn_fwd, INPUT_PULLUP);
  pinMode(btn_bwd, INPUT_PULLUP);
  pinMode (btn_spd, INPUT_PULLUP);

  Serial.begin(9600);
}

int state_spd_legacy = HIGH;

long repetition_counter = 0;
String legacy_print = "";
void print_with_counter(String print_text) {
  if (print_text == legacy_print) repetition_counter++;
  else repetition_counter=0;

  Serial.print(print_text);
  if (repetition_counter > 0) {
    Serial.print(" * ");
    Serial.print(repetition_counter); 
  }
  Serial.println();
  legacy_print = print_text;
}

void loop() {
  int state_fwd = digitalRead(btn_fwd);
  int state_bwd = digitalRead(btn_bwd);
  int state_spd = digitalRead(btn_spd);

  if (state_fwd == LOW && state_bwd == HIGH && state_spd == HIGH) {
    print_with_counter("move forward");
    motor.move_forward_single();
  }
  if (state_fwd == HIGH && state_bwd == LOW && state_spd == HIGH) {
    print_with_counter("move backward");
    motor.move_backward_single();
  }
  if (state_fwd == HIGH && state_bwd == HIGH && state_spd == LOW && state_spd_legacy != state_spd) {
    Serial.print("Set speed level to: ");
    int current_speed_level = motor.get_current_speed_level() + 1;
    if (current_speed_level > motor.get_number_of_speed_levels()) {
      current_speed_level = 1;
    }
    motor.set_speed_level(current_speed_level);
    Serial.println(current_speed_level);
  }
  state_spd_legacy = state_spd;
}
