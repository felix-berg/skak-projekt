#include "LCD_screen.hpp"
#include "Matrix_screen.hpp"

Matrix_screen matrix_scr { 10, 11, 12 };

void setup() {
	Serial.begin(9600);
	
 	matrix_scr.init();

	matrix_scr.pixels[0] = 0b00000000;
  matrix_scr.pixels[1] = 0b00000000;
  matrix_scr.pixels[2] = 0b00000000;
  matrix_scr.pixels[3] = 0b00000000;
  matrix_scr.pixels[4] = 0b00000000;
  matrix_scr.pixels[5] = 0b00000000;
  matrix_scr.pixels[6] = 0b00000000;
  matrix_scr.pixels[7] = 0b00000000;
}

void loop () {
  
	matrix_scr.update();
}
