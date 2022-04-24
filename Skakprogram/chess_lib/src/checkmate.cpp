#include "../include/primitives.hpp"
#include "../include/Valid_move.hpp"
#include "../include/checkmate.hpp"
#include "../include/Board.hpp"

int is_in_check(const Chess_Board & board, const Point & king_pos) {

	for (char y = 0; y < BOARD_HEIGHT; y++) {
		for (char x = 0; x < BOARD_WIDTH; x++) {
			Point other {x, y};
			if (board.color_of_piece(other) == board.color_of_piece(king_pos)) 
				continue;
			
			if (is_valid_move(board, other, king_pos))
				return IN_CHECK;
		}
	}
	return NOT_IN_CHECK;
}

int is_in_checkmate(const Chess_Board & board, const Point & king_pos) {
	if (is_in_check(board, king_pos)) {
		for (char y = king_pos.y - 1; y <= king_pos.y + 1; y++) 
			for (char x = king_pos.x - 1; x <= king_pos.x + 1; x++) 
				if (is_valid_move(board, king_pos, {x, y}) && !is_in_check(board, {x, y}))
					return IN_CHECK;

		return IN_CHECKMATE;
	} else 
		return NOT_IN_CHECK;
} 