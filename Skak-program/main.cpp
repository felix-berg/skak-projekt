#include "chess.hpp"

#include <wiringPi.h>
#include <wiringSerial.h>

#include <iostream>
#include <unistd.h>

#define INVALID_MOVE 0b00000000
#define ERROR_CODE   0b00000001
#define SUCCESS 		0b00000010

using byte = unsigned char;

int serialConnection = -1;
void init_serial()
{
	if ((serialConnection = serialOpen("/dev/ttyS0", 9600)) < 0)
      throw std::runtime_error("Error in initializing serial port.");   

   if (wiringPiSetup() == -1)
      throw std::runtime_error("Error in setting up wiring pi.");
   
   serialFlush(serialConnection);
}

void close_serial() {
	serialClose(serialConnection);
	serialConnection = -1;
}


Chess_Board::Piece_color currentColor = Chess_Board::Piece_color::WHITE;
Chess_Board * create_board()
{
	auto * ptr = new Chess_Board { };
	currentColor = Chess_Board::Piece_color::WHITE;
	return ptr;
}

void destroy_board(Chess_Board * board)
{
	delete board;
}

void send_byte(byte b)
{
	std::cout << "Sending byte: ";
	for (int i = 0; i < 8; i++)
		std::cout << bool(b & (0b10000000 >> i));
	std::cout << '\n';
	serialPutchar(serialConnection, b);
}

void respond_to_assert_valid(Chess_Board & b, char fx, char fy, char tx, char ty)
{
	Move user_move { b, Point {fx, fy}, Point {tx, ty} };

	send_byte((user_move.is_valid) ? SUCCESS : INVALID_MOVE);
}

void respond_to_move_req(Chess_Board & b, char fx, char fy, char tx, char ty)
{
	Point f { fx, fy },
			t { tx, ty };

	Move user_move { b, f, t };

	if (user_move.is_valid && currentColor == b.color_of_piece(f)) {
		b.do_move(user_move);
		send_byte(SUCCESS);
		currentColor = other_color(currentColor);
	} else
		send_byte(INVALID_MOVE);
}


void respond_to_get_req(Chess_Board & b, char x, char y)
{
	Point p { x, y };

	byte possible[8];
	get_possible_moves(possible, b, p);

	for (int i = 0; i < 8; i++)
		send_byte(possible[i]);
}

void respond_to_ai_req(Chess_Board & b, Chess_AI & ai)
{
	byte fromCode = 0b01000000;
	byte toCode   = 0b10000000;

	if (ai.get_color() == currentColor) {
		Move move { ai.get_best_move(b) };

		Point f = move.from;
		Point t = move.to;

		// do the move on the board
		b.do_move(move);

		// construct message |infocode (2)|x (3)|y (3)|
		// infocode is 01 for "from" position and 10 for "to" position
		byte fromByte = fromCode | (f.x << 3) | f.y;
		byte toByte   = toCode   | (t.x << 3) | t.y;

		send_byte(fromByte);
		send_byte(toByte);

		currentColor = other_color(currentColor);

	} else {
		std::cout << "not ai's turn\n";
		send_byte(ERROR_CODE);
	}
}

void respond_to_reset()
{
	destroy_board(g_board);
	g_board = create_board();

	if (g_board != nullptr)
		send_byte(SUCCESS);
	else 
		send_byte(ERROR_CODE);
}

void respond_to_get_all_pieces() {

}

/* Rules for incoming data: 
 * |header (2)|(data) (6)|
 * Header (2 bytes):
 	* 00: get possible moves
		* expects two following 3-bit msgs for x, then y
	* 01: move from (and then wait for move to)
		* expects two following 3-bit msgs for x, then y
	* 10: move to 
		* expects two following 3-bit msgs for x, then y
	* 11: allow ai to move, and return move 
		* no data
	* SPECIAL CASES:
		* 0b11111110: get all piece positions
		* 0b11111111: reset board
*/

void parse_byte(byte b, byte & opCode, char & x, char & y)
{
	opCode = (b & 0b11000000) >> 6;
	x 	  = (b & 0b00111000) >> 3;
	y 	  = (b & 0b00000111);
}

int main() {
	std::cout << "going\n";

	float ai_offense = 2.0f,
			ai_defense = 0.5f;

	Chess_AI ai { Chess_Board::Piece_color::BLACK, ai_defense, ai_offense };

	Chess_Board * g_board = create_board();

	init_serial();
	
	char fromX = -1, 
		 fromY = -1;


	while (true) {
		if (serialDataAvail(serialConnection) > 0) {
			byte data = serialGetchar(serialConnection);

			std::cout << "Request\n";

			// special case: reset board
			if (data == 0b11111111) {
				respond_to_reset();
				// continue while loop - escape if statement
				continue;
			} else if (data == 0b11111110) {
				respond_to_get_all_pieces();
				continue;
			}

			char x, y;
			byte opCode;
			parse_byte(data, opCode, x, y);

			std::cout << "Got command: (" << (int) opCode << ", (" << (int) x << ", " << (int) y  << ")\n";

			switch(opCode) {
				case 0b00:
					respond_to_get_req(*g_board, x, y);
					break;
				case 0b01:
					fromX = x;
					fromY = y;
					break;
				case 0b10:
					respond_to_move_req(*g_board, fromX, fromY, x, y);
					break;
				case 0b11:
					respond_to_ai_req(*g_board, ai);
					break;
			}

			print_board(g_board->pieces);
		}
	}


	close_serial();

	return 0;
}



/*
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
}*/