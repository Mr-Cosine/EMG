#include <Arduino.h>

#define button1 7
#define button2 8
#define button3 9

#define pressed(buttonPin) (buttonPin == LOW)

void setup() {
  Serial.begin(9600);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  Serial.print("start");
}

void loop() {
  int state1 = digitalRead(button1);
  int state2 = digitalRead(button2);
  int state3 = digitalRead(button3);

  if (pressed(state1)) {
    Serial.println("pressed button1");
  }
  if (pressed(state2)) {
    Serial.println("pressed button2");
  }
  if (pressed(state3)) {
    Serial.println("pressed button3");
  }

}
