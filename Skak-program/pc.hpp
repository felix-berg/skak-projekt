#include "../chess_lib/include/defines.hpp"
#include "../chess_lib/include/Board.hpp"

#include <string>
#include <iostream>

std::string piece_chars[13] = {
	"_", "♟", "♞", "♝", "♜", "♛", "♚",
		  "♙", "♘", "♗", "♖", "♕", "♔"
};

/*
	Prints an 8x8 chess board with given piece id's
*/
void print_board(char board[BOARD_WIDTH][BOARD_HEIGHT]) {
	printf("    A   B   C   D   E   F   G   H \n");
	printf("   _______________________________\n");
	for (int i = 7; i >= 0; i--) {
		std::cout << i + 1 << " |";
		for (int j = 0; j < 8; j++) {
			// print accociated character with given piece id
			char piece_id = board[i][j];
			printf(" ");

			// blank squares
			if (piece_id == 0) {
				if ((i + j) % 2 == 0)
					std::cout << WHITE_SQUARE;
				else 
					std::cout << BLACK_SQUARE;
			} else {
				// Prints characters assoiciated with given id.
				std::cout << piece_chars[piece_id];
			}
			printf(" |");
		}
		std::cout << "\n";
		std::cout << "  |___|___|___|___|___|___|___|___|\n";
	}
}

/*
	Prints chess board with XpieceX around valid moves.
*/
void print_possible_moves(char board[BOARD_WIDTH][BOARD_HEIGHT], bool possible_moves[BOARD_WIDTH][BOARD_HEIGHT]) {
	printf("    A   B   C   D   E   F   G   H \n");
	printf("   _______________________________\n");
	for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
		// std::cout << "  |   |   |   |   |   |   |   |   |\n";
		std::cout << i + 1 << " |";
		for (int j = 0; j < BOARD_WIDTH; j++) {
			// print accociated character with given piece id
			char piece_id = board[i][j];
			if (possible_moves[i][j])
				printf("x");
			else
				printf(" ");

			// blank squares
			if (piece_id == 0) {
				if ((i + j) % 2 == 0)
					std::cout << WHITE_SQUARE;
				else 
					std::cout << BLACK_SQUARE;
			} else {
				std::cout << piece_chars[piece_id];
			}
			// prints an x if the move is possible
			if (possible_moves[i][j])
				printf("x|");
			else
				printf(" |");
		}
		std::cout << "\n";
		std::cout << "  |___|___|___|___|___|___|___|___|\n";
	}
}

void get_move_from_user(Point & from, Point & to) {
	char from_str[2];
	char to_str[2];
	std::cin >> from_str[0] >> from_str[1] >> to_str[0] >> to_str[1];
	from = point_from_name(from_str);
	to = point_from_name(to_str);
}

void get_pos_from_user(Point & p) {
	char str[2];
	std::cin >> str[0] >> str[1];
	p = point_from_name(str);
}