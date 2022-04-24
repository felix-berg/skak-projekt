#include "../include/Board.hpp"
#include "../include/move.hpp"
#include "../include/Valid_move.hpp"

#include <memory.h>

/*
	Creates chess board based on given input char array
*/
Chess_Board::Chess_Board(char in_pieces[BOARD_HEIGHT][BOARD_WIDTH])
{
	// copy the in_pieces to pieces.
	for (int i = 0; i < BOARD_WIDTH; i++) {
		memcpy(pieces[i], in_pieces[i], sizeof(char) * BOARD_WIDTH);
	}
	memset(m_enp_vulnerable_pawns, false, sizeof(bool) * BOARD_HEIGHT * BOARD_WIDTH);
}

// default constructor -> creates regular starting position.
Chess_Board::Chess_Board() 
	: pieces {
	//  A  B  C  D  E  F  G  H
/*1*/	{10, 8, 9, 11, 12, 9, 8, 10},
/*2*/	{7, 7, 7, 7, 7, 7, 7, 7},
/*3*/	{0, 0, 0, 0, 0, 0, 0, 0},
/*4*/	{0, 0, 0, 0, 0, 0, 0, 0},
/*5*/	{0, 0, 0, 0, 0, 0, 0, 0},
/*6*/	{0, 0, 0, 0, 0, 0, 0, 0},
/*7*/	{1, 1, 1, 1, 1, 1, 1, 1},
/*8*/	{4, 2, 3, 5, 6, 3, 2, 4}
	}
{
	// initialize the en-passant array.
	memset(m_enp_vulnerable_pawns, false, sizeof(bool) * BOARD_HEIGHT * BOARD_WIDTH);
}

/*
	Get the number of pieces on the board.
*/
char Chess_Board::get_num_pieces() const {
	int count = 0; 
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			if (pieces[i][j] != NO_PIECE)
				count++;
		}
	}
	return count;
}

Chess_Board::Piece_color Chess_Board::color_of_piece(const Point & p) const {
	if (get_piece_id(p) >= B_PAWN)
		return Piece_color::BLACK;
	else if(get_piece_id(p) >= W_PAWN)
		return Piece_color::WHITE;
	else	
		return Piece_color::NO_COLOR;
}

void Chess_Board::set_piece(const Point & p, char piece_id) {
	if (piece_id < 0 || piece_id > B_KING) {
		printf("Error: Not a valid piece ID.\n");
		exit(1);
	}
	if (p.x < 0 || p.x >= BOARD_HEIGHT || p.y < 0 || p.y >= BOARD_WIDTH) {
		printf("Error: Position out of bounds.\n");
		exit(1);
	}
	pieces[p.y][p.x] = piece_id;
}

Point Chess_Board::get_king_pos(Piece_color color) const {
	char king_id = (color == Piece_color::WHITE) ? W_KING : B_KING;
	for (char y = 0; y < BOARD_HEIGHT; y++)
		for (char x = 0; x < BOARD_WIDTH; x++) 
			if (get_piece_id({x, y}) == king_id)
				return {x, y};
			
	printf("Error: Didn't find a king of the specified color.\n");
	exit(1);
}

void Chess_Board::update_king_moved(const Move & move) {
	if (get_piece_id(move.from) == W_KING)
		white_king_moved = true;
	if (get_piece_id(move.from) == B_KING)
		black_king_moved = true;
}

Move get_rook_castling_move(const Chess_Board & board, const Move & king_move) {
	// calculate the parameters of the moving king
	char king_moved_x = king_move.to.x - king_move.from.x;
	bool king_moved_right = king_moved_x > 0;
	char king_id = board.get_piece_id(king_move.from);

	// identify the position of the rook, based on the movement of the king
	char rook_row = (king_id == W_KING) ? ROW_8 : ROW_1;
	char rook_col = (king_moved_right) ? COL_H : COL_A;
	Point rook_from {rook_col, rook_row};

	// rook will move in the opposite direction of the king
	// 2 columns, if we are on the right side of the board, and 3 columns, if we are on the left side of the board
	int rook_moved_x = (king_moved_right) ? -2 : 3;
	Point rook_to = {(char) (rook_col + rook_moved_x), rook_row};

	Move rook_move {board, rook_from, rook_to};
	rook_move.is_valid = true; // this should be valid, since the king's move is valid, so no checks.

	return rook_move;
}

/*
	Check whether the given move is a castling king, and do a corresponding rook move, if so.
*/
void castle_king(Chess_Board & board, const Move & move) {
	Move rook_move = get_rook_castling_move(board, move);
	board.do_move(rook_move);
}

void undo_castle_king(Chess_Board & board, const Move & move) {
	Move rook_move = get_rook_castling_move(board, move);
	board.do_move(rook_move);

	// turn move around, make it the opposite.
	Point tmp = rook_move.to;
	rook_move.to = rook_move.from;
	rook_move.from = tmp;

	board.do_move(rook_move);
}

bool is_en_passant_attack(const Move & move) {
	// is the taken_piece_enp a valid value (not -1, -1), and 
	return move.taken_piece_enp != Point {(char) -1, (char) -1};
}

void Chess_Board::clear_en_passant_array() {
	for (int i = 0; i < BOARD_HEIGHT; i++)
		memset(m_enp_vulnerable_pawns[i], false, sizeof(bool) * BOARD_WIDTH);
}
/*
	Modifies the board according to the given move.
	Returns the piece_id of the taken piece
*/
char Chess_Board::do_move(const Move & move) {
	if (move.is_valid) {
		// allows for back-tracking "undo_move"
		char taken_piece_id = get_piece_id(move.to);

		update_king_moved(move);
		if (move.is_castling_king)
			castle_king(*this, move);
		
		// move the piece
		set_piece(move.to, get_piece_id(move.from));
		set_piece(move.from, NO_PIECE);

		// if this is an en passant move, remove the "taken_piece_enp".
		if (is_en_passant_attack(move))
			set_piece(move.taken_piece_enp, NO_PIECE);

		// clear "enpassantability" for all pieces
		clear_en_passant_array();

		// set en passant vulnerability, based on the given move.
		m_enp_vulnerable_pawns[move.to.y][move.to.x] = move.becomes_en_passantable;

		return taken_piece_id;

	} else {
		printf("Error: Attempt to do invalid move.\n");
		exit(1);
	}
}


void Chess_Board::undo_move(const Move & move, char taken_piece_id) {
	set_piece(move.from, get_piece_id(move.to));
	set_piece(move.to, taken_piece_id);

	if (move.is_castling_king) {
		undo_castle_king(*this, move);
	}

	if (is_en_passant_attack(move)) {
		if (color_of_piece(move.from) == Piece_color::WHITE)
			set_piece(move.taken_piece_enp, B_PAWN);
		else
			set_piece(move.taken_piece_enp, W_PAWN);
		
		m_enp_vulnerable_pawns[move.taken_piece_enp.y][move.taken_piece_enp.x] = true;
	}

	if (this->is_en_passantable(move.to))
		m_enp_vulnerable_pawns[move.to.y][move.to.x] = false;
}

/*
	Getter function for white_king_moved and black_king_moved.
*/
bool Chess_Board::has_king_moved(Chess_Board::Piece_color color) const {
	if (color == Chess_Board::Piece_color::WHITE)
		return white_king_moved;
	else if (color == Chess_Board::Piece_color::BLACK)
		return black_king_moved;
	else {
		printf("Error: Not a valid king color.\n");
		exit(1);
	}
}

void Chess_Board::del_piece(const Point & p) {
	set_piece(p, NO_PIECE);
}

void Chess_Board::switch_turns() {
	if (turn == Piece_color::WHITE)
		turn = Piece_color::BLACK;
	if (turn == Piece_color::BLACK)
		turn = Piece_color::WHITE;
}