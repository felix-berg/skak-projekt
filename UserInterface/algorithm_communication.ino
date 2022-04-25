#include "algorithm_communication.hpp"

void throw_error(const char *);

byte construct_algo_command(CommandType code, byte x, byte y) 
{
  if (x >= 8 || y >= 8) {
      throw_error("ALG: OB [0-7]");
  }

   // special cases
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

// to be declared in main file
void display_waiting_animation();

// same as above, but with animation
void wait_for_algo_response_animation(int responseLength) 
{
   unsigned long st = millis();   
   while (Serial.available() < responseLength) {
      if (millis() - st > g_maxResponseMillis) {
         throw_error("No reponse");
      }

      display_waiting_animation();
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

void wait_and_read_algo_response_to_array(byte * arr, int responseLength)
{
   wait_for_algo_response(responseLength);

   for (int i = 0; i < responseLength; i++)
      arr[i] = Serial.read();
   
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

   // send from, then to
   send_byte_to_algorithm(fb);
   send_byte_to_algorithm(tb);

   wait_and_check_for_success();
}

void get_possible_moves(byte * arr, int x, int y)
{
   byte cmd = construct_algo_command(GET_POSSIBLE, x, y);

   send_byte_to_algorithm(cmd);

   // wait for 8 bytes to return
   int responseLength = 8;
   wait_and_read_algo_response_to_array(arr, responseLength);
}

void get_all_pieces(byte * arr)
{
   byte cmd = construct_algo_command(GET_ALL_PIECES, 0, 0);

   send_byte_to_algorithm(cmd);

   // wait until entire response has been sent
   int responseLength = 8;
   wait_and_read_algo_response_to_array(arr, responseLength);
}

void get_ai_move_from_algorithm(int * fx, int * fy, int * tx, int * ty)
{
   byte request = construct_algo_command(AI, 0, 0); // x, y are arbitrary
   send_byte_to_algorithm(request);

   // wait until algorithm responds with 2 bytes
   wait_for_algo_response(2);

   byte from, to;

   // get from, then to from algorithm
   from = Serial.read();
   to   = Serial.read();

   // extract x and y from gotten message
   *fx = (from & 0b111000) >> 3;
   *fy = (from & 0b111);
   *tx = (to   & 0b111000) >> 3;
   *ty = (to   & 0b111);

   // throw away uneeded bytes
   Serial.flush();
}