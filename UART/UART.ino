#define UART_PACKET_TYPE byte
#include "UART.hpp"

uart::Reciever srx {2, 9600};
uart::Transmitter stx {3, 9600};

void setup()
{
   Serial.begin(9600);
   // to initialize the reciever and transmitter module
   srx.init();
   stx.init();
}

void loop()
{
   if (!srx.isEmpty()) {
    uart::Packet p = srx.read_packet();
    Serial.print((char) p.msg); Serial.print(": ");
    for (int i = 0; i < sizeof(p.msg) * 8; i++)
      Serial.print(bool((0b10000000 >> i) & p.msg));
    Serial.println();
   }
   
   stx.update();
}
