#include "../include/Board.hpp"
#include "../include/Valid_move.hpp"
#include "../include/Piece_rules.hpp"
#include "../include/move.hpp"

/*
	Checks whether a certain move on the board is valid, after chess standard rules.
	Sets taken_piece to the piece, that will be taken through the move.
	Sets will_be_en_passantable to boolean true/false, based on whether the "from" piece becomes vulnerable to en passant on the next turn. Expects, that board has valid information about which pawns are currently en_passantable.
*/
bool construct_valid_move(const Chess_Board & board, const Point & f, const Point & t, Point & taken_piece_enp, bool & will_be_en_passantable, bool & castling_king) {
	
	will_be_en_passantable = false; // whether the move is an en passant manouver
	taken_piece_enp = {(char) -1, (char) -1}; // taken piece, if an en passant happened
	castling_king = false;

	// are you trying to move out of board?
	if (!point_on_board(f) || !point_on_board(t))
		return false;

	// Is "from" not a piece?
	if (board.get_piece_id(f) == NO_PIECE)
		return false;
	
	// is this a collision with a teammate?
	if (collides_with_team(board, f, t))
		return false;

	// are we moving at all?
	if (f == t)
		return false;

	// check the piece rules for every given piece.
	switch (board.get_piece_id(f)) {
		case W_PAWN:
			return Piece_rules::is_valid_pawn_move(board, f, t, taken_piece_enp, will_be_en_passantable);
			break;
		case B_PAWN:
			return Piece_rules::is_valid_pawn_move(board, f, t, taken_piece_enp, will_be_en_passantable);
			break;
		case W_BISHOP: case B_BISHOP:
			return Piece_rules::is_valid_bishop_move(board, f, t);
			break;
		case W_KNIGHT: case B_KNIGHT:
			return Piece_rules::is_valid_knight_move(board, f, t);
			break;
		case W_ROOK: case B_ROOK:
			return Piece_rules::is_valid_rook_move(board, f, t);
			break;
		case W_QUEEN: case B_QUEEN:
			return Piece_rules::is_valid_queen_move(board, f, t);
			break;
		case W_KING: case B_KING:
			return Piece_rules::is_valid_king_move(board, f, t, castling_king);
			break;
		default:
			printf("Error: Invalid piece to be evaluated.\n");
			exit(1);
			return false;
			break;
	}
}

/*
	Checks whether a given position on the board is within the range 0-BOARD_WIDTH in x and 0-BOARD_HEIGHT in y.
*/
bool point_on_board(const Point & p) {
	bool is_not_on_board = p.x < 0 || p.x >= BOARD_WIDTH || p.y < 0 || p.y >= BOARD_HEIGHT;
	return !is_not_on_board;
}

/*
	Checks if f and t are the same color.
*/
bool collides_with_team(const Chess_Board & board, const Point & f, const Point & t) {
	return board.get_piece_id(t) != NO_PIECE &&
			 board.color_of_piece(f) == board.color_of_piece(t);
}

/*
	Checks any given from and to positions, and checks, whether that given move is valid on the board.
*/
bool is_valid_move(const Chess_Board & board, const Point & f, const Point & t) {
	Point decoy_taken_enp;
	bool decoy_enpassantable;
	bool decoy_castling_king;
	return construct_valid_move(board, f, t, decoy_taken_enp, decoy_enpassantable, decoy_castling_king);
}
/*
	Gets all possible moves on the given board from the given position.
*/
void get_possible_moves(bool possible[BOARD_HEIGHT][BOARD_WIDTH], Chess_Board & board, const Point & p) {
	for (int i = 0; i < BOARD_HEIGHT; i++)
		for (int j = 0; j < BOARD_WIDTH; j++) {
			possible[i][j] = is_valid_move(board, p, {(char) j, (char) i});
		}
}


void get_possible_moves(unsigned char possible[BOARD_HEIGHT], Chess_Board & board, const Point & p)
{
	static_assert(BOARD_WIDTH == 8);

	for (int i = 0; i < BOARD_HEIGHT; i++) {
		unsigned char row = 0;
		for (int j = 0; j < BOARD_WIDTH; j++) {
			bool b = is_valid_move(board, p, { (char) j, (char) i});
			unsigned char c = b ? (0b10000000 >> j) : 0;
			row = row | c;
		}
		
		possible[i] = row;
	}
}