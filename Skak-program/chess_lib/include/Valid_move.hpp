#ifndef VALID_MOVE_HPP
#define VALID_MOVE_HPP
#include "defines.hpp"

class Chess_Board;
class Point;

bool collides_with_team(const Chess_Board & board, const Point & f, const Point & t);
bool point_on_board(const Point & p);

void get_possible_moves(bool possible[BOARD_WIDTH][BOARD_HEIGHT], Chess_Board & board, const Point & p);
void get_possible_moves(unsigned char possible[BOARD_HEIGHT], Chess_Board & board, const Point & p);
bool construct_valid_move(const Chess_Board & board, const Point & f, const Point & t, Point & taken_piece, bool & enpassantable, bool & castling_king);
bool is_valid_move(const Chess_Board & board, const Point & f, const Point & t);


#endif