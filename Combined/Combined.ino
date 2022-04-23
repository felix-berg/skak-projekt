
#define UART_PACKET_TYPE byte
#include "UART.hpp"

uart::Reciever    uartRX { 2, 9600 };
uart::Transmitter uartTX { 3, 9600 };

void setup()
{
  Serial.begin(9600);
  
  uartRX.init();
  uartTX.init();
}

#define FROM true
#define TO   false

void print_byte(byte b)
{
  for (int i = 0; i < 8; i++)
    Serial.print(bool(b & (0b10000000 >> i)));
}

void print_byte_ln(byte b)
{
  print_byte(b);
  Serial.println();
}

byte construct_command(bool is_from, byte x, byte y) 
{
  if (x >= 8 || y >= 8) {
    Serial.println("Error: Can't move outside [0-7] range");
    delay(100);
    exit(1);
  }

  return
    (is_from ? 0b01000000 : 0b10000000) // Op code
      |
    (x << 3) // x-coordinate
      |
     y;      // y-coordinate
}

bool from = true;
void loop()
{

  while (Serial.available() > 0) {
    Serial.read();
    
      byte b;
    if (from) {
      b = construct_command(FROM, 0, 0);
      from = false;
    } else {
      b = construct_command(TO, 7, 7);
      from = true;
    }
      
    print_byte_ln(b);

    uartTX.send({b});
  }

  uartTX.update();
}



















/*
#include "Matrix_screen.hpp"
#include "LCD_screen.hpp"
#include "inputs.hpp"

Matrix_screen mtx { 13, 12, 11 };
LCD_screen lcd { };
Buttons buttons { 7, 6 };

void rotate_bits_r(byte & b) 
{
  byte remaining_bit = (b & 0b00000001) << 7;
  b >>= 1;
  
  b |= remaining_bit;
}
void rotate_bits_l(byte & b)
{
  
}

void setup() {
  Serial.begin(9600);
  mtx.init();
  lcd.init();
  buttons.init();

  mtx.pixels[0] = 0b10001000;
  mtx.pixels[1] = 0b01000100;
  mtx.pixels[2] = 0b00100010;
  mtx.pixels[3] = 0b00010001;
  mtx.pixels[4] = 0b10001000;
  mtx.pixels[5] = 0b01000100;
  mtx.pixels[6] = 0b00100010;
  mtx.pixels[7] = 0b00010001;

  lcd.set_str("yep", 0);

  pinMode(8, OUTPUT);
}

unsigned long lastUpdate = 0;
unsigned long interval = 200;

int row = 0;

void loop () {

  digitalWrite(8, HIGH);
  delayMicroseconds(10);
  digitalWrite(8, LOW);

  byte b = buttons.poll();

  for (int i = 0; i < 8; i++)
   mtx.pixels[i] = b;
  
  if (b) {
    lcd.set_str("Knap trykket!", 0);
    lcd.clear_row(1);
  } else {
    lcd.set_str("Knap ikke", 0);
    lcd.set_str("trykket!", 1);
  }
  
  mtx.update(); 
  lcd.update();
}*/
