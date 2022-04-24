#include "UART.hpp"

typedef unsigned long u_long;

using namespace uart;

u_long g_delayTime  = -1;
u_long g_startDelay = -1;
int g_rxPin = -1;
int g_numInterrupts = 0;

int g_interruptTimeThreshold = -1;

Reciever * globalReceiver;


volatile u_long g_lastPacketRecieve = 0;

void uart::on_get_bit() 
{
   noInterrupts(); // avoid doubling of 

   u_long mcs = micros();
   if (mcs - g_lastPacketRecieve > g_interruptTimeThreshold) {
      g_lastPacketRecieve = mcs;
   } else return;

   Packet result;
   result.msg = 0;

   // offset, so we read in the middle, plus waiting for the start bit
   delayMicroseconds(g_startDelay);

   // read every bit of the incoming byte.  
   for (int i = uart::Packet::n_bits - 1; i >= 0; i--) {
      result.msg = result.msg | (digitalRead(g_rxPin) << i); // add byte and shift 
      delayMicroseconds(g_delayTime);
   }

   // wait for end bit
   if (digitalRead(g_rxPin) != 0)
      Serial.println("Error: Didn't read correct end bit."); // use Serial.begin() in setup, if you want these error checks

   delayMicroseconds(g_delayTime);
   globalReceiver->push_packet(result);
}

void uart::Reciever::init() 
{
   if (globalReceiver != nullptr)  {
      Serial.println("Cannot have multiple recievers at once.");
      exit(1);
   }

   globalReceiver = this;

   const unsigned long one_second = 1000000;
   const long attach_offset = 10; // measured delay for attach to happen.

   // length of each pulse
   g_delayTime = float(one_second) / float(baudrate);

   // delay for starting bit + offset for reading next bit.
   g_startDelay = g_delayTime + float(g_delayTime) / 2.0f - attach_offset;

   g_interruptTimeThreshold = g_delayTime * (Packet::n_bits * 2 + 1);

   g_rxPin = rxPin;

   pinMode(g_rxPin, INPUT);
   Serial.print("Setting digital pin "); Serial.print(g_rxPin); Serial.println(" to interrupt on rising pulse.");
   attachInterrupt(digitalPinToInterrupt(g_rxPin), on_get_bit, RISING);
}

void uart::Reciever::unload() 
{
   if (globalReceiver)
      globalReceiver = nullptr;
   
   g_delayTime = -1;
   g_startDelay = -1;
   g_rxPin = -1;

   detachInterrupt(digitalPinToInterrupt(rxPin));
}
