#define INPUTPIN 8
#define CLOCKPIN 9

#include "inputs.hpp"

Buttons buttons { INPUTPIN, CLOCKPIN };

void setup() {
	Serial.begin(9600);
	buttons.init();
}

void loop() {
	buttons.poll();

	for (int i = 0; i < 8; i++) {
		Serial.print(buttons.key_is_pressed(i));
	}
	Serial.println();
 
  delay(100);
}
