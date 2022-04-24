#ifndef CHECK_HPP
#define CHECK_HPP

#define NOT_IN_CHECK 0
#define IN_CHECK 1
#define IN_CHECKMATE 2

#include "Board.hpp"

class Point;

#include <iostream>

bool is_in_check(const Chess_Board & board, Chess_Board::Piece_color color);
// int is_in_checkmate(const Chess_Board & board, const Point & pos);

#endif