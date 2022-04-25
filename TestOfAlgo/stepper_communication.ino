#include "stepper_communication.hpp"

// to be declared in main file
void throw_error(const char * str);
void display_waiting_animation();

byte construct_stepper_command(FromTo ft, int x, int y) 
{
  if (x >= 8 || y >= 8) {
    throw_error("STP: OB [0-7]");
  }

  return
    ((ft == FROM) ? 0b01000000 : 0b10000000) // Op code
      |
    (x << 3) // x-coordinate
      |
     y;      // y-coordinate
}


const unsigned long maxMoveTime = 40000;
void wait_for_stepper_response(uart::Reciever & uRX)
{
   unsigned long st = millis();
   // wait until a packet has arrived
   while (uRX.isEmpty()) {
      if (millis() - st > maxMoveTime) {
         throw_error("STP: No res");
      }

      // update user interface while waiting, since it may take a while
      display_waiting_animation();
   };
}

#define SUCCESS 0b00000010
void move_piece_from_to(uart::Reciever & uRX, uart::Transmitter & uTX, int fx, int fy, int tx, int ty)
{
   byte fb = construct_stepper_command(FROM, fx, fy);
   byte tb = construct_stepper_command(TO, tx, ty);

   uTX.send(Packet { fb });
   uTX.send(Packet { tb });

   // push all pending packets
   while (uTX.has_pending())
      uTX.update();

   // wait until packet available on RX pin.
   // indicating, that stepper has finished moving
   wait_for_stepper_response(uRX);

   // read the resulting byte -> succes??
   byte b = uRX.read_packet().msg;
   if (b != SUCCESS)
      throw_error("STP: No sccss");
}