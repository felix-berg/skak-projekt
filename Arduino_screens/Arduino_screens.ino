#include "LCD_screen.hpp"
#include "Matrix_screen.hpp"

LCD_screen lcd;

void setup() {
	Serial.begin(9600);
	
 	lcd.init();
	
	lcd.set_str("Hello, ", 0);
	lcd.set_str("World!", 1);
}

void loop () {
	lcd.update();
}
