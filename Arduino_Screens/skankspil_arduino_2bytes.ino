#include "Matrix_screen.hpp"
#include "LCD_screen.hpp"

Matrix_screen mtx { 11, 12, 13 };
LCD_screen lcd { };


void rotate_bits_r(byte & b) 
{
  byte remaining_bit = (b & 0b00000001) << 7;
  b >>= 1;
  
  b |= remaining_bit;
}

void setup() {
  Serial.begin(9600);
  mtx.init();
  lcd.init();

  mtx.pixels[0] = 0b10001000;
  mtx.pixels[1] = 0b01000100;
  mtx.pixels[2] = 0b00100010;
  mtx.pixels[3] = 0b00010001;
  mtx.pixels[4] = 0b10001000;
  mtx.pixels[5] = 0b01000100;
  mtx.pixels[6] = 0b00100010;
  mtx.pixels[7] = 0b00010001;

  lcd.set_str("Gamer", 0);
}

unsigned long lastUpdate = 0;
unsigned long interval = 200;

int row = 0;

void loop () {
	
  
  if (millis() - lastUpdate > interval) {
    for (int i = 0; i < 8; i++)
      rotate_bits_r(mtx.pixels[i]);
      
    lastUpdate = millis();
  }
  mtx.update(); 
  lcd.update();
}
