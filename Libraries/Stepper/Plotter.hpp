#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include "CalibratingStepper.hpp"

class Plotter {

  CalibratingStepper x_axis;
  CalibratingStepper y_axis;

public:
  Plotter(int x_dir, int x_step, int x_sleep, int x_button, ButtonSide x_buttonSide,
          int y_dir, int y_step, int y_sleep, int y_button, ButtonSide y_buttonSide,
          int steps_per_resolution, int steps_per_second)
    : x_axis { x_dir, x_step, x_sleep, steps_per_resolution, steps_per_second, x_button, x_buttonSide },
      y_axis { y_dir, y_step, y_sleep, steps_per_resolution, steps_per_second, y_button, y_buttonSide }
  { };

  void init()
  {
    x_axis.init();
    y_axis.init();
  }

  void calibrate() 
  {
    x_axis.calibrate();
    y_axis.calibrate();
  }

  void move_to(int x, int y)
  {
    if (x < 0 || x >= CalibratingStepper::maxSteps ||
        y < 0 || y >= CalibratingStepper::maxSteps)
    {
      Serial.println("Error: Step target out of range.");
      delay(100);
      exit(1);
    }
    
    x_axis.step_to(x);
    y_axis.step_to(y);
  }

  void update()
  {
    x_axis.update();
    if (x_axis.at_target())
      y_axis.update();
  }

  bool at_target() const { return x_axis.at_target() && y_axis.at_target(); };

  int get_speed() const 
  {
    return x_axis.get_speed();
  }

  void set_speed(int spd)
  {
    x_axis.set_speed(spd);
    y_axis.set_speed(spd);
  }

  int xpos() const { return x_axis.current_step(); };
  int ypos() const { return y_axis.current_step(); };

  void xpos(int x) { x_axis.step_to(x); };
  void ypos(int y) { y_axis.step_to(y); };
};

#endif
