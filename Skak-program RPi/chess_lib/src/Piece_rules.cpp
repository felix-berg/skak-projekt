#include "../include/Piece_rules.hpp"
#include "../include/Valid_move.hpp"
#include "../include/defines.hpp"
#include "../include/Board.hpp"
#include "../include/move.hpp"

#include <cmath> 

/*
	Every rule given is based on what is "not allowed", since this is thought to
	provide a simpler way to test the given move, rather than working through every special case. 
	
	Every rule will return a boolean, true if the move is valid, in terms of standard chess rules, and false otherwise.
*/


//		A B C D E F G H
// 7 |_|_|_|_|_|_|_|_| 
// 7 |_|_|_|_|_|_|_|_| 
// 6 |_|_|_|_|_|_|_|_|  
// 5 |_|_|_|_|_|_|_|_|
// 4|_|_|_|(M)|_|_|_|_|
// 3 |_|_|O|M|O|_|_|_|
// 2 |_|_|_|P|_|_|_|_|
// 1 |_|_|_|_|_|_|_|_|
bool Piece_rules::is_valid_pawn_move(const Chess_Board & board, const Point & f, const Point & t, Point & taken_piece_enp, bool & will_be_enpassantable) {
	taken_piece_enp = {-1, -1};
	will_be_enpassantable = false;

	int moved_x = t.x - f.x;
	int moved_y = t.y - f.y;


	//******************************************************* 
	// FOR WHITE PIECES

	int one_forward, two_forward, pawn_row;

	if (board.get_piece_id(f) == W_PAWN) {
		one_forward = -1;
		two_forward = -2;
		pawn_row 	 = ROW_7;
	} else if (board.get_piece_id(f) == B_PAWN) {
		one_forward = 1;
		two_forward = 2;
		pawn_row = ROW_2;
	} else 
		return false;

	// are we not only moving 1 forward?
	if (moved_y != one_forward) {
		// are we moving 2 forward?
		if (moved_y != two_forward)
			return false;

		// if so, are we on the pawn row?
		if (f.y != pawn_row) 
			return false;

		// we are on the pawn row -> is anyone in the way?
		Point middle {f.x, (char) (f.y + one_forward)};
		if (board.get_piece_id(middle) != NO_PIECE)
			return false;

		// are we blocked in front of us?
		if (moved_x == 0 && board.get_piece_id(t) != NO_PIECE)
			return false;

		// we are successfully making an opening pawn move -> we are "enpassantable"
		will_be_enpassantable = true;
	} 

	// are we moving vertically?
	if (moved_x != 0) {
		// are we moving with more than one
		if (moved_y != one_forward)
			return false;

		// is there not an attackable piece at "t"
		if (board.get_piece_id(t) == NO_PIECE) {
			// EN PASSANT SPECIAL CASE
			if (board.is_en_passantable({(char) (f.x + moved_x), f.y}) && 
				 board.get_piece_id({(char) (f.x + moved_x), f.y}) != board.get_piece_id(f)) 
			{
				taken_piece_enp = {(char) (f.x + moved_x), f.y};
			} else {
				// not a valid en passant move
				return false;
			}
		}
		// if we more than one
		if (moved_x != -1 && moved_x != 1)
			return false;
	}


	if (moved_y == one_forward && moved_x == 0 && board.get_piece_id(t) != NO_PIECE)
		return false;

	return true;
}

//		A B C D E F G H
// 7 |M|_|_|_|_|_|_|_| 
// 7 |_|M|_|_|_|_|_|M| 
// 6 |_|_|M|_|_|_|M|_|  
// 5 |_|_|_|M|_|M|_|_|
// 4 |_|_|_|_|B|_|_|_|
// 3 |_|_|_|M|_|M|_|_|
// 2 |_|_|M|_|_|_|M|_|
// 1 |_|M|_|_|_|_|_|M|
bool Piece_rules::is_valid_bishop_move(const Chess_Board & board, const Point & f, const Point & t) {
	int moved_x = t.x - f.x;
	int moved_y = t.y - f.y;

	float move_gradient = (float) moved_x / (float) moved_y;
	
	// did we not move diagonally?
	if (abs(move_gradient) != 1.0f) 
		return false;
	
	// is anyone in the way?
	// move along the way to t-point, and check if we find anything else than a free space, before we meet t.

	// amount to add every step
	int step_dir_x = moved_x / abs(moved_x);
	int step_dir_y = moved_y / abs(moved_y);

	Point check = {
		(char) (f.x + step_dir_x), 
		(char) (f.y + step_dir_y)};
	
	while (check != t) {
		if (board.get_piece_id(check) != NO_PIECE)
			return false;

		check.x += step_dir_x;
		check.y += step_dir_y;
	}
	return true;
}

//		A B C D E F G H
// 7 |_|_|_|_|_|_|_|_| 
// 7 |_|_|_|_|_|_|_|_| 
// 6 |_|_|_|M|_|M|_|_|  
// 5 |_|_|M|_|_|_|M|_|
// 4 |_|_|_|_|K|_|_|_|
// 3 |_|_|M|_|_|_|M|_|
// 2 |_|_|_|M|_|M|_|_|
// 1 |_|_|_|_|_|_|_|_|
bool Piece_rules::is_valid_knight_move(const Chess_Board & board, const Point & f, const Point & t) {
	int moved_x = t.x - f.x;
	int moved_y = t.y - f.y;

	// checks the L-shaped moves
	bool legal = (abs(moved_x) == 2 && abs(moved_y) == 1) ||
					 (abs(moved_x) == 1 && abs(moved_y) == 2);
	if (!legal)
		return false;

	return true;
}

//		A B C D E F G H
// 8 |_|_|_|_|M|_|_|_| 
// 7 |_|_|_|_|M|_|_|_|
// 6 |_|_|_|_|M|_|_|_|
// 5 |_|_|_|_|M|_|_|_|
// 4 |M|M|M|M|Q|M|M|M|
// 3 |_|_|_|_|M|_|_|_|
// 2 |_|_|_|_|M|_|_|_|
// 1 |_|_|_|_|M|_|_|_|
bool Piece_rules::is_valid_rook_move(const Chess_Board & board, const Point & f, const Point & t) {
	int moved_x = t.x - f.x;
	int moved_y = t.y - f.y;

	// checks, that only one coordinate is changed
	bool legal = (moved_x == 0 || moved_y == 0);
	if (!legal)
		return false;

	// is anyone in the way?
	// move along the way to t-point, and check if we find anything else than a free space, before we meet t.
	// amount to add every step
	int step_dir_x = (moved_x == 0) ? 0 : moved_x / abs(moved_x);
	int step_dir_y = (moved_y == 0) ? 0 : moved_y / abs(moved_y);

	Point check = {
		(char) (f.x + step_dir_x), 
		(char) (f.y + step_dir_y)};
	
	while (check != t) {
		if (board.get_piece_id(check) != NO_PIECE)
			return false;

		check.x += step_dir_x;
		check.y += step_dir_y;
	}

	return true;
}

//		A B C D E F G H
// 8 |M|_|_|_|M|_|_|M| 
// 7 |_|M|_|_|M|_|_|M|
// 6 |_|_|M|_|M|_|M|_|
// 5 |_|_|_|M|M|M|_|_|
// 4 |M|M|M|M|Q|M|M|M|
// 3 |_|_|_|M|M|M|_|_|
// 2 |_|_|M|_|M|_|M|_|
// 1 |_|M|_|_|M|_|_|M|
bool Piece_rules::is_valid_queen_move(const Chess_Board & board, const Point & f, const Point & t) {
	return Piece_rules::is_valid_bishop_move(board, f, t) ||
			 Piece_rules::is_valid_rook_move(board, f, t);
}

//		A B C D E F G H
// 8 |_|_|_|_|_|_|_|_|
// 7 |_|_|M|M|M|_|_|_|
// 6 |_|_|M|K|M|_|_|_|
// 5 |_|_|M|M|M|_|_|_|
// 4 |_|_|_|_|_|_|_|_|
// 3 |_|_|_|+|_|_|_|_|
// 2 |_|_|_|_|_|_|_|_|
// 1 |R|_|M|_|K|_|M|R|
bool Piece_rules::is_valid_king_move(const Chess_Board & board, const Point & f, const Point & t, bool & is_castling_king) {
	is_castling_king = false;
	int moved_x = t.x - f.x;
	int moved_y = t.y - f.y;

	if (abs(moved_x) > 1 || abs(moved_y) > 1) {
		// we are moving irregularly (more than one radius)
		// are we castling?
		char king_id = board.get_piece_id(f); 
		// get specific attributes for white or black king
		char king_row = (king_id == W_KING) ? ROW_8 : ROW_1; 
		bool king_moved;
		if (king_id == W_KING) 
			king_moved = board.has_king_moved(Chess_Board::Piece_color::WHITE);
		else 
			king_moved = board.has_king_moved(Chess_Board::Piece_color::WHITE);
		
		char rook_id = (king_id == W_KING) ? W_ROOK : B_ROOK;

		// if the king hasn't moved yet
		if (!king_moved) {
			// do the rules of castling apply: right side case
			if (board.get_piece_id({COL_H, king_row}) == rook_id &&
				 board.get_piece_id({COL_G, king_row}) == NO_PIECE &&
				 board.get_piece_id({COL_F, king_row}) == NO_PIECE &&
				 t == Point {COL_G, king_row}) {
					 
				is_castling_king = true;
			// left side case
			} else if (board.get_piece_id({COL_A, king_row}) == rook_id &&
						  board.get_piece_id({COL_B, king_row}) == NO_PIECE &&
						  board.get_piece_id({COL_C, king_row}) == NO_PIECE &&
						  board.get_piece_id({COL_D, king_row}) == NO_PIECE &&
						  t == Point {COL_C, king_row}) {
				
				is_castling_king = true;
			} else
				// the rules of castling did not apply
				return false;
		} else 
			return false;
	}
	return true;
}
