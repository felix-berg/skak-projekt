#ifndef MOVE_HPP
#define MOVE_HPP

#include "Board.hpp"

struct Move {
	Move() { };
	Move(const Chess_Board & board, const Point & f, const Point & t);

	Point from, to;
	// taken piece in en passant attack. will always be {-1, -1}, unless an en passant move has been initiated
	Point taken_piece_enp = {(char) -1, (char) -1};
	
	bool becomes_en_passantable = false;
	bool is_castling_king = false;
	bool is_valid = false;
};

#endif