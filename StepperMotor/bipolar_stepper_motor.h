#ifndef BIPOLAR_STEPPER_MOTOR_H
#define BIPOLAR_STEPPER_MOTOR_H

#include "Arduino.h"
#include <stdint.h>

typedef uint16_t length_t;
typedef uint8_t level_t;

#ifndef LOW
#define LOW 0
#endif
#ifndef HIGH
#define HIGH 1
#endif

#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x1
#endif
#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif

#ifndef default_iterator
#define default_iterator _QW5keSBpcyBmdWNraW5nIGF3ZXNvbWUh  // change this to whatever you like (make sure don't use this naming in any other iterators in the loop)
#endif

#define repeat_times(times) for(int default_iterator=0; default_iterator<(times); default_iterator++)
#define repeat_times_w_counter(counter, times) for (uint32_t (counter)=0; (counter)<(times); (counter)++)
#define repeat_until(condition) while(!(condition))
#define repeat_while(condition) while(condition)
#define repeat_forever while(true)

class Bipolar_stepper_motor {
  struct Ref_point { 
    /*  
    * @param value The numeric level displayed
    * @param delay The time between each step of motor actuation
    */
    level_t value;
    float interstep_delay;

    Ref_point(level_t value, float delay):
      value(value),
      interstep_delay(delay)
    {}
  };

  class Speed_levels {
    /*  
    * @param valmax The highest level of speed
    * @param delaymin The lowest delay per step (determines how fast the highest speed level is)
    * @param delaymax The highest delay per step (determines how slow the lowest speed level is)
    */
    private:
      Ref_point max_speed;
      Ref_point min_speed;
    public:
      Speed_levels(level_t valmax, float delaymin, float delaymax):
        max_speed(Ref_point(valmax, delaymin)),
        min_speed(Ref_point(1, delaymax))
      {}

      float interpolate_delay_ms(level_t);

      level_t max_speed_level();
      void set_max_speed_level(level_t);
      void set_interstep_delay_min_ms(float);
      void set_interstep_delay_max_ms(float);
  };

  private:
    uint8_t IN1, IN2, IN3, IN4;
    length_t actuation_length_forward; // in cycles, need to be measured and adjusted accordingly
    length_t actuation_length_backward; // in cycles, need to be measured and adjusted accordingly
    level_t current_speed_level;
    Speed_levels speed_levels;

    void setStep(bool a1, bool a2, bool b1, bool b2) {
      digitalWrite(this->IN1, a1);
      digitalWrite(this->IN2, a2);
      digitalWrite(this->IN3, b1);
      digitalWrite(this->IN4, b2);
    }
    void resetStep() {
      digitalWrite(this->IN1, LOW);
      digitalWrite(this->IN2, LOW);
      digitalWrite(this->IN3, LOW);
      digitalWrite(this->IN4, LOW);
    }
    void cycle_forward(length_t);
    void cycle_backward(length_t);

  public:
    // CONSTRUCTORS
    Bipolar_stepper_motor(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4):
      IN1(IN1), IN2(IN2), IN3(IN3), IN4(IN4),
      actuation_length_forward(0),
      actuation_length_backward(0),
      current_speed_level(1),
      speed_levels(Speed_levels(10, 5, 10)) // by default: 10 speed levels,  5ms min delay, 10ms max delay.
    {
      pinMode(this->IN1, OUTPUT);
      pinMode(this->IN2, OUTPUT);
      pinMode(this->IN3, OUTPUT);
      pinMode(this->IN4, OUTPUT);
    }

    level_t get_number_of_speed_levels() { return this->speed_levels.max_speed_level(); }
    level_t get_current_speed_level() { return this->current_speed_level; }
    length_t get_forward_actuation_length() { return this->actuation_length_forward; }
    length_t get_backward_actuation_length() { return this->actuation_length_backward; }

    // CONFIGURATIONS
    void set_number_of_speed_levels(level_t);
    void set_speed_level(level_t);
    void set_multiplied_actuation_length(float);
    void set_all_actuation_length_based_on_forward(length_t);
    void set_all_actuation_length_based_on_backward(length_t);
    void set_forward_actuation_length(length_t);
    void set_backward_actuation_length(length_t);
    void set_interstep_delay_min_ms(float);
    void set_interstep_delay_max_ms(float);

    // ACTUATIONS
    void move_forward_single();
    void move_forward_single(level_t);
    void move_backward_single();
    void move_backward_single(level_t);
    void move_forward();
    void move_forward(level_t);
    void move_backward();
    void move_backward(level_t);
};

#endif