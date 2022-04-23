#ifndef UART_HPP
#define UART_HPP

#include <QueueArray.h>

// User defined type
#ifndef UART_PACKET_TYPE
#error No packet type defined. Usage example: #define UART_PACKET_TYPE unsigned int
#endif

namespace uart {
   struct Packet {
      static constexpr int n_bits = sizeof(UART_PACKET_TYPE) * 8;
      UART_PACKET_TYPE msg;
   };

   /** Only works with one receiver at a time */
   class Reciever {
   public:
      const unsigned long baudrate;
      const int rxPin;

      Reciever(int rx, int bdrate) : rxPin { rx }, baudrate { bdrate } { }

      void init();
      void unload();
      int num_packets() const { return m_packetBuffer.count(); };
      
      bool isEmpty() const { return num_packets() == 0; };

      Packet read_packet() 
      {
         if (num_packets() <= 0) {
            Serial.println("Cannot pop empty buffer.\n");
            return Packet { NULL };
         }

         return m_packetBuffer.dequeue();;
      }
   private:
      QueueArray<Packet> m_packetBuffer;

      friend void uart::on_get_bit();

      void push_packet(const Packet & packet) 
      {
         m_packetBuffer.enqueue(packet);
      };

   };

   void on_get_bit();

   class Transmitter {
   public:
      const unsigned long baudrate;
      const int txPin;
 
      Transmitter(int tx, int bdrate) : txPin { tx }, baudrate { bdrate } { };

      void init() {
         pinMode(txPin, OUTPUT);

         m_delayTime = 1000000.0f / float(baudrate);

         m_timeBetweenPackets = m_delayTime * (uart::Packet::n_bits * 2); // leeway for receiver to pick up new interrupt
      };

      void unload();

      void send(const Packet & p) { 
         m_packetBuffer.enqueue(p);
      };

      void update() {
         if (micros() - m_lastSendTime > m_timeBetweenPackets) {
            send_pending_packets();
            m_lastSendTime = micros();
         }
      };

   private:
      unsigned long m_delayTime;
      unsigned long m_timeBetweenPackets;
      unsigned long m_lastSendTime = 0;

      QueueArray<Packet> m_packetBuffer;

      void send_bit(bool b) {
         digitalWrite(txPin, b);
         delayMicroseconds(m_delayTime);
      }

      void send_pending_packets() {
         if (m_packetBuffer.count() > 0) {
            Packet p = m_packetBuffer.dequeue();
            
            // start bit
            send_bit(HIGH);
      
            for (int i = uart::Packet::n_bits - 1; i >= 0; i--) {
               bool b = (0b1 << i) & p.msg;
               send_bit(b);
            }

            send_bit(LOW);

         }
      };
   };

   void print_packet(const Packet & p) {
      for (int i = Packet::n_bits - 1; i >= 0; i--) {
         bool b = (0b1 << i) & p.msg;
         Serial.print(b);
      }
   }

};

#endif