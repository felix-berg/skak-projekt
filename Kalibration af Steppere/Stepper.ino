#include "CalibratingStepper.hpp"

CalibratingStepper sx { 7, 6, 5, 200, 400, 13, ButtonSide::RIGHT };
CalibratingStepper sy { 10, 9, 8, 200, 400, 12, ButtonSide::LEFT };

void setup() {
  Serial.begin(9600);
  sx.init();
  sy.init();

  Serial.println("start");

  delay(2000);

  sx.calibrate();
  sy.calibrate();

  sx.map_target(1, 0, 100);
  sy.map_target(99, 0, 100);

  delay(1000);
}

void loop() {  
  sx.update();
  sy.update();
}


/*
Stepper sx {10, 9, 8, 200, 400};

unsigned long last_switch = 0;

void setup() {
  Serial.begin(9600);
  sx.init();

  Serial.println("start");

  sx.move_target(200 * 2);

  Serial.println(sx.get_speed());
}

void loop() {

 
  sx.update();
}*/
