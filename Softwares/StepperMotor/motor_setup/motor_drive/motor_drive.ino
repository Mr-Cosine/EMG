#include "bipolar_stepper_motor.h"

int IN1 = 2;
int IN2 = 3;
int IN3 = 4;
int IN4 = 5;

Bipolar_stepper_motor motor(IN1, IN2, IN3, IN4);

void setup() {
  motor.set_number_of_speed_levels(20);
  motor.set_interstep_delay_min_ms(3);
  motor.set_speed_level(20);

  Serial.begin(9600);
  delay(50);
  Serial.println("READY");
}

void loop() {
  handleSerialCommands();
}

// Protocol: one line per command, newline-terminated, comma-separated.
//   FWD           - move forward one single step
//   BWD           - move backward one single step
//   SPD,<n>       - set speed level
//   DMIN,<ms>     - set interstep delay at max speed (ms)
//   DMAX,<ms>     - set interstep delay at min speed (ms)
// Every command replies "OK" on success, "ERR ..." otherwise.
void handleSerialCommands() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  int commaIdx = line.indexOf(',');
  String cmd = (commaIdx == -1) ? line : line.substring(0, commaIdx);
  String argStr = (commaIdx == -1) ? "" : line.substring(commaIdx + 1);

  if (cmd == "FWD") {
    repeat_times(10) {motor.move_forward_single();}
    Serial.println("OK");
  } 
  else if (cmd == "BWD") {
    repeat_times(10) {motor.move_backward_single();}
    Serial.println("OK");
  } 
  else if (cmd == "DMIN") {
    if (argStr.length() == 0) {
      Serial.println("ERR DMIN needs a value");
    } else {
      motor.set_interstep_delay_min_ms(argStr.toFloat());
      Serial.println("OK");
    }
  } 
  else if (cmd == "DMAX") {
    if (argStr.length() == 0) {
      Serial.println("ERR DMAX needs a value");
    } else {
      motor.set_interstep_delay_max_ms(argStr.toFloat());
      Serial.println("OK");
    }
  } 
  else if (cmd == "PIN1") {
    if (argStr.length() == 0) {
      Serial.println("ERR PIN1 needs a value");
    } else {
      IN1 = argStr.toInt();
      motor.set_pin_IN1(IN1);
      Serial.println("OK");
    }
  } 
  else if (cmd == "PIN2") {
    if (argStr.length() == 0) {
      Serial.println("ERR PIN2 needs a value");
    } else {
      IN2 = argStr.toInt();
      motor.set_pin_IN2(IN2);
      Serial.println("OK");
    }
  } 
  else if (cmd == "PIN3") {
    if (argStr.length() == 0) {
      Serial.println("ERR PIN3 needs a value");
    } else {
      IN3 = argStr.toInt();
      motor.set_pin_IN3(IN3);
      Serial.println("OK");
    }
  } 
  else if (cmd == "PIN4") {
    if (argStr.length() == 0) {
      Serial.println("ERR PIN4 needs a value");
    } else {
      IN4 = argStr.toInt();
      motor.set_pin_IN4(IN4);
      Serial.println("OK");
    }
  }

  else {
    Serial.println("ERR unknown command");
  }
}
