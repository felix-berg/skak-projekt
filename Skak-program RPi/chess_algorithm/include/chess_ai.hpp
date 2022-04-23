#ifndef CHESS_AI
#define CHESS_AI

#include "../../chess_lib/include/Board.hpp"
#include "../../chess_lib/include/move.hpp"
#include "digraph.hpp"

#include <vector>

class Chess_AI {
	public:
		static const int search_depth = 3;
		Chess_AI(Chess_Board::Piece_color col, float defense, float offense) : m_color { col }, m_defense { defense }, m_offense { offense } { };
		Chess_AI(Chess_Board::Piece_color col) : Chess_AI { col, 1.0f, 1.0f }{ };

		int get_score(Node &);

		void initialize_node(Node * node, Chess_Board & board, int depth);
		float get_children_average_material_difference(Node * node, int depth);

		Move get_best_move(Chess_Board &);

		Chess_Board::Piece_color get_color() const { return m_color; };

	private:
		const Chess_Board::Piece_color m_color;

		float m_offense = 0.0f, 
				m_defense = 0.0f;

		std::vector<char> m_taken_pieces;
};



#endif