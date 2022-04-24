#ifndef CALIBRATING_STEPPER_HPP
#define CALIBRATING_STEPPER_HPP

#include "Stepper.hpp"

enum ButtonSide { LEFT, RIGHT };

class CalibratingStepper : public Stepper {
   int m_buttonPin;
   ButtonSide m_side;

   static const int calibrationSpeed = 50;
   
public:
   static const int maxSteps = 1950; // measured value

   CalibratingStepper(int dir, int stepp, int sleep, int steps_per_resolution, int steps_per_second, int button, ButtonSide buttonSide)
      : Stepper { dir, stepp, sleep, steps_per_resolution, steps_per_second },
         m_buttonPin { button }, m_side { buttonSide } 
   { };

   void calibrate()
   {
      int oldSpeed = get_speed();
      set_speed(calibrationSpeed);

      int stepDirection = (m_side == RIGHT) * 2 - 1; // either -1 one (for button at left side) or 1 (at right side)

      // move one step at a time, stopping, if we have pressed the button
      while (!digitalRead(m_buttonPin)) {
         if (at_target()) 
            m_target_step += stepDirection;
         
         this->update();
      }

      m_current_step = (m_side == RIGHT) ? maxSteps : 0;
      m_target_step  = m_current_step;

      set_speed(oldSpeed);
   }

   void step_to(int step)
   {
      // bounds-checking
      if (step < 0 || step >= maxSteps) {
         Serial.println("Error: Can't step CalibratingStepper outside of range [0:maxSteps).");
         delay(100);
         exit(1);
      }

      Stepper::step_to(step);
   }

   void move_target(int to)
   {
      step_to(m_target_step + to);
   }

   void map_target(int val, int minVal, int maxVal)
   {
      if (val > maxVal || val < minVal) {
         Serial.println("Error: Value out of range [minVal; maxVal]");
         exit(1);
      }
      int desiredStep = map(val, minVal, maxVal, 0, maxSteps);

      step_to(desiredStep);
   }
};

#endif
