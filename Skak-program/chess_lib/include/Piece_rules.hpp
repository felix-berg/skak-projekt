#ifndef PIECE_RULES_HPP
#define PIECE_RULES_HPP

class Move;
class Chess_Board;
class Point;

namespace Piece_rules {
	bool is_valid_pawn_move(const Chess_Board & board, const Point & f, const Point & t, Point & taken_piece_enp, bool & will_be_enpassantable);
	bool is_valid_bishop_move(const Chess_Board & board, const Point & f, const Point & t);
	bool is_valid_knight_move(const Chess_Board & board, const Point & f, const Point & t);
	bool is_valid_rook_move(const Chess_Board & board, const Point & f, const Point & t);
	bool is_valid_queen_move(const Chess_Board & board, const Point & f, const Point & t);
	bool is_valid_king_move(const Chess_Board & board, const Point & f, const Point & t, bool & castling_king);
};
#endif