#include "../include/move.hpp"
#include "../include/Valid_move.hpp"
#include "../include/defines.hpp"


Move::Move(const Chess_Board & board, const Point & f, const Point & t) 
	: from { f }, to { t }
{
	is_valid = construct_valid_move(board, from, to, taken_piece_enp, becomes_en_passantable, is_castling_king);
}