#include "chess_lib/include/defines.hpp"
#include "chess_lib/include/Board.hpp"
#include "chess_lib/include/Valid_move.hpp"
#include "chess_lib/include/move.hpp"
#include "chess_lib/include/check.hpp"

#include "chess_algorithm/include/chess_ai.hpp"
#include "chess_algorithm/include/digraph.hpp"

#include "PC_Chess_IO/pc.hpp"


#include <iostream>
int main() {
	// initialize chess board with default table of pieces.
	Chess_Board main_board { };

	print_board(main_board.pieces);

	// weights for killing opponent vs. staying alive. Factor, so 1.0f and 1.0f would be "neutral".
	float ai_offense = 2.0f,
			ai_defense = 0.5f;

	// initiate the chess ai
	Chess_AI ai { Chess_Board::Piece_color::BLACK, ai_defense, ai_offense };

	std::cout << "Welcome to chess. You own the white pieces!\n\n";

	while (true) {
		std::cout << "Move command: m XX XX. Get possible moves command: g XX.\n";

		char control_char;
		std::cin >> control_char;
		if (control_char == 'm') {
			Point f, t;
			get_move_from_user(f, t);

			// initialize instance of "move" class, which can only create valid moves.
			Move user_move {main_board, f, t};

			// check for invalid color 
			if (!user_move.is_valid) {
				std::cout << "Invalid move!\n";
				continue; // let user go again, and dont move anything
			}

			// do the given (valid) move
			main_board.do_move(user_move);
			print_board(main_board.pieces);
			
			if (is_in_check(main_board, Chess_Board::BLACK))
				std::cout << "Black is in check!";
			if (is_in_check(main_board, Chess_Board::WHITE))
				std::cout << "White is in check!";

			std::cout << "\nComputing countermove...\n\n";
			Move ai_move = ai.get_best_move(main_board);

			main_board.do_move(ai_move);
			print_board(main_board.pieces);

			if (is_in_check(main_board, Chess_Board::BLACK))
				std::cout << "Black is in check!";
			if (is_in_check(main_board, Chess_Board::WHITE))
				std::cout << "White is in check!";
			

		} else if (control_char == 'g') {

			bool possible_moves[BOARD_WIDTH][BOARD_HEIGHT] = {{0}};
			Point pos;
			get_pos_from_user(pos);

			get_possible_moves(possible_moves, main_board, pos);
			print_possible_moves(main_board.pieces, possible_moves);

		}
	}
}