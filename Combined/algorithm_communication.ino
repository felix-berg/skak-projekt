#include "algorithm_communication.hpp"

void throw_error(const char *);
                  //    00          01       10    11 ...
byte construct_algo_command(CommandType code, byte x, byte y) 
{
  if (x >= 8 || y >= 8) {
    Serial.println("Error: Can't move outside [0-7] range");
    delay(100);
    exit(1);
  }

   if (code == CommandType::GET_ALL_PIECES)
      return 0b11111110;
   else if (code == CommandType::RESET)
      return 0b11111111;

  return
    (code << 6) // Op code
      |
    (x << 3) // x-coordinate
      |
     y;      // y-coordinate
}

void send_byte_to_algorithm(byte & b)
{
   char * c = (char *) &b;
   Serial.print(*c);
}

#define INVALID_MOVE 0b00000000
#define ERROR_CODE   0b00000001
#define SUCCESS      0b00000010

unsigned long g_maxResponseMillis = 10000;
void wait_for_algo_response(int responseLength)
{
   unsigned long st = millis();   
   while (Serial.available() < responseLength) {
      if (millis() - st > g_maxResponseMillis) {
         throw_error("No reponse");
      }
   };
}

void wait_and_check_for_success()
{
   wait_for_algo_response(1);

   // read a byte... was it a success?
   if (Serial.read() != SUCCESS) {
      throw_error("No success");
   }

   // throw away uneeded bytes
   Serial.flush();
}

void send_restart_command()
{
   byte b = construct_algo_command(RESET, 0, 0); // arbitrary x and y

   send_byte_to_algorithm(b);

   wait_and_check_for_success();
}

void send_move_to_algorithm(int fx, int fy, int tx, int ty)
{
   byte fb = construct_algo_command(SET_FROM, fx, fy);
   byte tb = construct_algo_command(SET_TO,   tx, ty);

   send_byte_to_algorithm(fb);
   send_byte_to_algorithm(tb);

   wait_and_check_for_success();
}

void get_possible_moves(byte * arr, int x, int y)
{
   byte cmd = construct_algo_command(GET_POSSIBLE, x, y);

   send_byte_to_algorithm(cmd);

   // wait for 8 bytes to return
   wait_for_algo_response(8);

   for (int i = 0; i < 8; i++)
      arr[i] = Serial.read();
   
   Serial.flush();
}

void get_all_pieces(byte * arr)
{
   byte cmd = construct_algo_command(GET_ALL_PIECES, 0, 0);

   send_byte_to_algorithm(cmd);

   wait_for_algo_response(8);

   for (int i = 0; i < 8; i++)
      arr[i] = Serial.read();
   
   Serial.flush();
}

void get_ai_move_from_algorithm(int * fx, int * fy, int * tx, int * ty)
{
   byte request = construct_algo_command(AI, 0, 0); // x, y are arbitrary

   send_byte_to_algorithm(request);

   wait_for_algo_response(2);

   byte from, to;

   from = Serial.read();
   to   = Serial.read();

   *fx = (from & 0b111000) >> 3;
   *fy = (from & 0b111);
   *tx = (to   & 0b111000) >> 3;
   *ty = (to   & 0b111);

   // throw away uneeded bytes
   Serial.flush();
}