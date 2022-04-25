#include "algorithm_communication.hpp"
#include "stepper_communication.hpp"
#include "LCD_screen.hpp"
#include "Matrix_screen.hpp"

#define UART_PACKET_TYPE byte
#include "UART.hpp"

LCD_screen lcd { };
Matrix_screen mtx { 13, 12, 11 };

void setup() 
{
   Serial.begin(9600);
   lcd.init();
   mtx.init();

   mtx.pixels[0] = 0b11111111;

   lcd.set_str("start", 0);

   lcd.update();
   mtx.update();

   send_restart_command();

   lcd.set_str("1", 0);
   lcd.update();

   send_move_to_algorithm(4, 6, 4, 4);


   lcd.set_str("2", 0);
   lcd.update();

   int fx, fy, tx, ty;
   get_ai_move_from_algorithm(&fx, &fy, &tx, &ty);


   lcd.set_str("3", 0);
   lcd.update();

   // send_move_to_algorithm(1, 7, 2, 5);

   lcd.set_str("4", 0);
   lcd.update();

   // get_ai_move_from_algorithm(&fx, &fy, &tx, &ty);


   lcd.set_str("5", 0);
   lcd.update();

   byte pieces[8];
   get_all_pieces(pieces);


   lcd.set_str("6", 0);
   lcd.update();
   delay(1000);

   for (int i = 0; i < 8; i++)
      mtx.pixels[i] = pieces[i];
   mtx.update();


}

void loop()
{
   


   mtx.update();
}

void throw_error(const char * s)
{
   lcd.set_str(s, 1);
   lcd.update();
   delay(2000);
   exit(1);
}