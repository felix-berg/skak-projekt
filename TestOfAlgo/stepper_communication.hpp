#pragma once

#define UART_PACKET_TYPE byte
#include "UART.hpp"

enum FromTo { FROM, TO };
void move_piece_from_to(uart::Reciever & uRX, uart::Transmitter & uTX, int fx, int fy, int tx, int ty);