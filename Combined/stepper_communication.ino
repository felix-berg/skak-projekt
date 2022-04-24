#include "stepper_communication.hpp"

void throw_error(const char * str);

byte construct_stepper_command(FromTo ft, int x, int y) 
{
  if (x >= 8 || y >= 8) {
    Serial.println("Error: Can't move outside [0-7] range");
    delay(100);
    exit(1);
  }

  return
    ((ft == FROM) ? 0b01000000 : 0b10000000) // Op code
      |
    (x << 3) // x-coordinate
      |
     y;      // y-coordinate
}

#define SUCCESS 0b00000010
const unsigned long maxMoveTime = 40000;
void move_piece_from_to(uart::Reciever & uRX, uart::Transmitter & uTX, int fx, int fy, int tx, int ty)
{
   byte fb = construct_stepper_command(FROM, fx, fy);
   byte tb = construct_stepper_command(TO, tx, ty);

   uTX.send(Packet { fb });
   uTX.send(Packet { tb });

   while (uTX.has_pending())
      uTX.update();

   unsigned long st = millis();
   // wait until a packet has arrived
   while (uRX.isEmpty()) {
      if (millis() - st > maxMoveTime) {
         throw_error("STP: No res");
      }
   };

   byte b = uRX.read_packet().msg;
   if (b != SUCCESS)
      throw_error("STP: No sccss");
}