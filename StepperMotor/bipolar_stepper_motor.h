#ifndef BIPOLAR_STEPPER_MOTOR_H
#define BIPOLAR_STEPPER_MOTOR_H

#include "Arduino.h"
#include <stdint.h>

#ifndef default_iterator
#define default_iterator _QW5keSBpcyBmdWNraW5nIGF3ZXNvbWUh  // change this to whatever you like (make sure don't use this naming in any other iterators in the loop)
#endif

#define repeat_times(times) for(int default_iterator=0; default_iterator<(times); default_iterator++)
#define repeat_times_w_counter(counter, times) for (int (counter)=0; (counter)<(times); (counter)++)
#define repeat_until(condition) while(!condition)
#define repeat_while(condition) while(condition)
#define repeat_forever while(true)

class Bipolar_stepper_motor {
  struct Ref_point { 
    /*  
    * @param value: the numeric level displayed
    * @param delay: the time between each step of motor actuation
    */
    int value;
    double interstep_delay;

    Ref_point(int value, double delay):
      value(value),
      interstep_delay(delay)
    {}
  };

  class Speed_levels {
    /*  
    * @param valmax: the highest level of speed
    * @param delaymin: the lowest delay per step (determines how fast the highest speed level is)
    * @param delaymax: the highest delay per step (determines how slow the lowest speed level is)
    */
    private:
      Ref_point max_speed;
      Ref_point min_speed;
    public:
      Speed_levels(int valmax, double delaymin, double delaymax):
        max_speed(Ref_point(valmax, delaymin)),
        min_speed(Ref_point(1, delaymax))
      {}

      double interpolate_delay_ms(int);

      int max_speed_level();
      void set_max_speed_level(int);
      void set_interstep_delay_min_ms(double);
      void set_interstep_delay_max_ms(double);
  };

  private:
    int IN1; int IN2; int IN3; int IN4;
    int actuation_length_forward; // in cycles, need to be measured and adjusted accordingly
    int actuation_length_backward; // in cycles, need to be measured and adjusted accordingly
    int current_speed_level;
    Speed_levels speed_levels;

    void setStep(bool a1, bool a2, bool b1, bool b2) {
      digitalWrite(this->IN1, a1);
      digitalWrite(this->IN2, a2);
      digitalWrite(this->IN3, b1);
      digitalWrite(this->IN4, b2);
    }

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

    int get_number_of_speed_levels() { return this->speed_levels.max_speed_level(); }
    int get_current_speed_level() { return this->current_speed_level; }
    int get_forward_actuation_length() { return this->actuation_length_forward; }
    int get_backward_actuation_length() { return this->actuation_length_backward; }

    // CONFIGURATIONS
    void set_number_of_speed_levels(int);
    void set_speed_level(int);
    void set_multiplied_actuation_length(double);
    void set_all_actuation_length_based_on_forward(int);
    void set_all_actuation_length_based_on_backward(int);
    void set_forward_actuation_length(int);
    void set_backward_actuation_length(int);
    void set_interstep_delay_min_ms(double);
    void set_interstep_delay_max_ms(double);

    // ACTUATIONS
    void move_forward_single();
    void move_backward_single();
    void move_forward();
    void move_forward(int);
    void move_backward();
    void move_backward(int);
};

#endif