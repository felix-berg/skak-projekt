#ifndef BOARD_HPP
#define BOARD_HPP
#include "defines.hpp"
#include "point.hpp"
class Move;
#include <iostream>
#include <vector>

typedef char Pieces[BOARD_HEIGHT][BOARD_WIDTH];

class Chess_Board {
	public:
		enum Piece_color {WHITE, BLACK, NO_COLOR};

		Chess_Board(char in_points[BOARD_HEIGHT][BOARD_WIDTH]);
		// creates normal starting board
		Chess_Board();

		char pieces[BOARD_HEIGHT][BOARD_WIDTH];

		// get functions
		char get_piece_id(const Point & p) const { return pieces[p.y][p.x]; };
		char get_num_pieces() const;
		bool is_en_passantable(const Point & p) const { return m_enp_vulnerable_pawns[p.y][p.x]; };
		Piece_color color_of_piece(const Point & p) const;
		Point get_king_pos(Piece_color color) const;
		bool has_king_moved(Piece_color color) const;

		// modifying functions
		char do_move(const Move & move);
		void undo_move(const Move & move, char taken_piece_id);
		void del_piece(const Point &);

	private:
		bool white_king_moved = false;
		bool black_king_moved = false;

		void clear_en_passant_array();
		bool m_enp_vulnerable_pawns[BOARD_HEIGHT][BOARD_WIDTH];
		Piece_color turn = Piece_color::WHITE; // whos turn it is

		void update_king_moved(const Move & );
		void set_piece(const Point & point, char piece_id);
		void switch_turns();


		std::vector<char> taken_piece_ids;
};

#endif