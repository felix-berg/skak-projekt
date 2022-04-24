#ifndef PC_HPP
#define PC_HPP

#include "../chess_lib/include/defines.hpp"

class Point;

#include <string>
extern std::string piece_chars[13];

void get_move_from_user(Point & from, Point & to);
void get_pos_from_user(Point & p);
void print_board(const char board [BOARD_HEIGHT][BOARD_WIDTH]);
void print_possible_moves(const char board [BOARD_HEIGHT][BOARD_WIDTH], bool possible_moves[BOARD_HEIGHT][BOARD_WIDTH]);

#endif