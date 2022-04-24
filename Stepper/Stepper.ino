#include "Plotter.hpp"
#define UART_PACKET_TYPE byte
#include "UART.hpp"

#define ELECTROMAGNETPIN 6

Plotter plotter { 
  7,  6, 5, 13, ButtonSide::RIGHT,
  10, 9, 8, 12, ButtonSide::LEFT,
  200, 300
};

uart::Reciever    uartRX { 2, 9600 };
uart::Transmitter uartTX { 3, 9600 };

void print_byte(byte b) {
  for (int i = 0; i < 8; i++) 
    Serial.print((bool) (b & (0b10000000 >> i)));
}

void print_byte_ln(byte b) {
  print_byte(b);
  Serial.println();
}

const int close_x   = 50;
const int close_y   = 50;
const int far_x     = 1900;
const int far_y     = 1900;

const int cell_size_x = (far_x - close_x) / 8;
const int cell_size_y = (far_y - close_y) / 8;

int times = 0;
void wait_until_moved(Plotter & p) {
  while (!p.at_target()) {
    if (times++ % 1000 == 0) {
      Serial.print(plotter.xpos()); 
      Serial.print(", "); 
      Serial.println(plotter.ypos());
    }    
    p.update();
  }
}

void move_piece_from_to(int fx, int fy, int tx, int ty)
{ 
  int dx = tx - fx;
  int dy = ty - fy;
  
  // true if direction of movement is positive in the x direction
  // false if negative
  bool dirX = dx > 0;
  bool dirY = dy > 0;

  // move under piece
  int fromX = close_x + cell_size_x / 2 + cell_size_x * fx;
  int fromY = close_y + cell_size_y / 2 + cell_size_y * fy;
  
  plotter.move_to(fromX, fromY);
  wait_until_moved(plotter);

  Serial.println("At from"); delay(200);

  // activate electromagnet
  digitalWrite(ELECTROMAGNETPIN, HIGH);

  // move out to vertical line besides cell
  int vertical_line_x = close_x + cell_size_x * (fx + dirX);
  plotter.move_to(vertical_line_x, fromY);
  wait_until_moved(plotter);
  
  Serial.println("Exited"); delay(200);

  // move to crossing between vertical and horisontal line
  int horisontal_line_y = close_y + cell_size_y * (ty + !dirY);
  plotter.move_to(vertical_line_x, horisontal_line_y);
  wait_until_moved(plotter);
  
  Serial.println("At crossing"); delay(200);

  // move over to besides cell
  int toX = close_x + cell_size_x / 2 + cell_size_x * tx;
  plotter.move_to(toX, horisontal_line_y);
  wait_until_moved(plotter);

  Serial.println("Entering"); delay(200);

  // move under cell -> done!
  int toY = close_y + cell_size_y / 2 + cell_size_y * ty;
  plotter.move_to(toX, toY);
  wait_until_moved(plotter);

  // deactivate electromagnet
  digitalWrite(ELECTROMAGNETPIN, LOW);
}


/** Message structure:
  * Header: 2 bytes
    * 01: Set from target
    * 10: Set to target
  * Body: 6 bytes (2 3 bit numbers indicating position on chess board)
    * |XXXYYY| 
*/
int fromX = -1, fromY = -1;
void react_to_command(byte b) {
  byte opCode = (b & 0b11000000) >> 6;
  byte x = (b & 0b00111000) >> 3;
  byte y =  b & 0b00000111;

  if (opCode == 0b01) {
    fromX = x;
    fromY = y;
  } else if (opCode == 0b10) {
    if (fromX != -1 && fromY != -1)
      move_piece_from_to(fromX, fromY, x, y);
  }
}

#define SUCCESS 0b00000010

void check_for_commands() {
  while (!uartRX.isEmpty()) {
    byte b = uartRX.read_packet().msg;

    react_to_command(b);
    uartTX.send(Packet {SUCCESS});
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(ELECTROMAGNETPIN, OUTPUT);
  plotter.init();

  Serial.println("Waiting 2000 ms...");

  delay(2000);

  Serial.println("Calibrating stepper motors...");

  //plotter.calibrate();

  delay(100);

  Serial.println("Ready for request!");

  uartRX.init();
  uartTX.init();
}

void loop() {
  check_for_commands();

  move_piece_from_to(3, 2, 5, 6);
  
  uartTX.update();
}


/*
Stepper sx {10, 9, 8, 200, 400};

unsigned long last_switch = 0;

void setup() {
  Serial.begin(9600);
  sx.init();

  Serial.println("start");

  sx.move_target(200 * 2);

  Serial.println(sx.get_speed());
}

void loop() {

 
  sx.update();
}*/
