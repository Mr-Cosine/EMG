#ifndef BIPOLAR_STEPPER_MOTOR_H
#define BIPOLAR_STEPPER_MOTOR_H

#include "Arduino.h"
#include <stdint.h>

#ifndef default_iterator
#define default_iterator _qwwdvdca
#endif

#define repeat_times(times) for(int default_iterator=0; default_iterator<(times); default_iterator++)
#define repeat_times_w_counter(counter, times) for (int (counter)=0; (counter)<(times); (counter)++)
#define repeat_until(condition) while(!condition)
#define repeat_while(condition) while(condition)
#define repeat_forever(condition) while(true)

class Bipolar_stepper_motor {
  struct Ref_point {
    int value;
    int interstep_delay;

    Ref_point(int value, int delay):
      value(value),
      interstep_delay(delay)
    {}
  };

  class Speed_levels {
    private:
      Ref_point max_speed;
      Ref_point min_speed;
    public:
      Speed_levels(int valmax, int delaymin, int delaymax):
        max_speed(Ref_point(valmax, delaymin)),
        min_speed(Ref_point(1, delaymax))
      {}

      double interpolate_delay_ms(int);
      int max_speed_level();
      void set_interstep_delay_min_ms(int);
      void set_interstep_delay_max_ms(int);
  };

  private:
    int IN1; int IN2; int IN3; int IN4;
    int actuation_length_forward; // in cycles, need to be measured and adjusted accordingly
    int actuation_length_backward;
    int current_speed_level;
    Speed_levels speed_levels;

    void setStep(bool, bool, bool, bool);

  public:
    // CONSTRUCTORS
    Bipolar_stepper_motor(int IN1, int IN2, int IN3, int IN4):
      IN1(IN1), IN2(IN2), IN3(IN3), IN4(IN4),
      actuation_length_forward(0),
      actuation_length_backward(0),
      current_speed_level(1),
      speed_levels(Speed_levels(10, 5, 10))
    {
      pinMode(this->IN1, OUTPUT);
      pinMode(this->IN2, OUTPUT);
      pinMode(this->IN3, OUTPUT);
      pinMode(this->IN4, OUTPUT);
    }

    // CONFIGURATIONS
    void set_number_of_speed_levels(int);
    void set_speed_level(int);
    void set_multiplied_actuation_length(double);
    void set_all_actuation_length_based_on_forward(int);
    void set_all_actuation_length_based_on_backward(int);
    void set_forward_actuation_length(int);
    void set_backward_actuation_length(int);
    void set_interstep_delay_min_ms(int);
    void set_interstep_delay_max_ms(int);

    // ACTUATIONS
    void move_forward_single();
    void move_forward_single(int);
    void move_backward_single();
    void move_backward_single(int);
    void move_forward();
    void move_forward(int);
    void move_backward();
    void move_backward(int);
};

#endif