#ifndef STEPPER_HPP
#define STEPPER_HPP

int num_pulses = 0;

class Stepper {
public:
  static const long time_until_sleep = 500000 ;
  static const int min_pulse_time = 385; // measured with no load

   Stepper(int dir, int stepp, int sleep, int steps_per_resolution, int steps_per_second)
      :  dir_pin { dir }, step_pin { stepp }, sleep_pin { sleep }, m_total_steps { steps_per_resolution } { 
          set_speed(steps_per_second);
      };

   void init();
   void update();

   /* Set target */
   void step_to(int step) { m_target_step = step; };

   /* Shift target */
   void move_target(int steps) { m_target_step += steps; };

   /* Returns number of steps per round. */
   int one_res() const { return m_total_steps; };

   /* Returns the current position. */ 
   int current_step() const { return m_current_step; };

   bool at_target() const { return m_current_step == m_target_step; };

   int get_speed() const { return 500000 / m_pulse_time; };
   void set_speed(int steps_per_sec) { m_pulse_time = 500000 / steps_per_sec; }; // integer division -> floor value
   void set_step_time(int t) { m_pulse_time = t / 2; };

   int sleep() { set_sleep(true ); };
   int wake()  { set_sleep(false); };

protected:
   void pulse();
   /* Set direction */
   void dir(bool d) { 
      if (m_direction == d)
         return;
      
      m_direction = d; 
      digitalWrite(dir_pin, m_direction);
   };	

   /* Set state of sleep pin */
   void set_sleep(bool s) {
      if (m_sleep == s)
         return;    
    
      m_sleep = s; 
      digitalWrite(sleep_pin, !m_sleep);
   }

   int step_pin, dir_pin, sleep_pin;
   int m_total_steps = 0;
   int m_target_step = 0;
   int m_current_step = 0;

   int m_pulse_time; // time in µs of half a step-time

private:
   bool m_step_pin_state = false;
   bool m_direction = true;
   bool m_sleep = true;
   unsigned long m_last_pulse;

};

/** To be called in the arduino setup() function. */
void Stepper::init() {
   // initiate pin outputs
   pinMode(step_pin, OUTPUT);
   pinMode(dir_pin,  OUTPUT);
   pinMode(sleep_pin, OUTPUT);

   // set default values to pins
   digitalWrite(dir_pin, m_direction);
   digitalWrite(step_pin, m_step_pin_state);

  set_sleep(false);
  
  m_last_pulse = micros();
}

/*
   Continue the stepping by doing a pulse.
   Should be called every half step-time. (m_pulse_time)
*/
void Stepper::pulse() {
   // switch step pin
   m_step_pin_state = !m_step_pin_state;
   digitalWrite(step_pin, m_step_pin_state);

   if (!m_step_pin_state) { // if this was a falling pulse, a step is done
      m_current_step += int(m_direction) * 2 - 1; // set to -1 or 1 depending on m_direction.
   }
}

/*
   Should be called in the arduino update() function.
   Steps the motor, if the given pulse-time has passed.
   Deals with m_current_step.
*/
void Stepper::update() {
   bool should_move = !at_target();

   if (should_move && micros() - m_last_pulse > m_pulse_time) {
      /* Move towards target */
      set_sleep(false);
    
      bool target_dir = m_target_step > m_current_step;
   
      if (target_dir != m_direction) {
        // if the direction is incorrect.
         dir(target_dir); // set the direction of the step
      }

      // switch the state of step_pin
      pulse();
      // reset timer 
      m_last_pulse = micros(); 
   }

   // after last step -> reset to 0
  if (!should_move && m_step_pin_state) {
    if (micros() - m_last_pulse > m_pulse_time) {
      pulse();
      // reset time
      m_last_pulse = micros(); 
    }
  }

   // --- disabled -> physics got in the way
//   if (micros() - m_last_pulse > time_until_sleep)
//     set_sleep(false);
}

#endif
