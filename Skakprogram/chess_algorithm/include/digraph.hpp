#ifndef DIGRAPH_HPP
#define DIGRAPH_HPP

class Move;
class Chess_Board;
struct Edge;

#include "../../chess_lib/include/move.hpp"
#include "../../chess_lib/include/Board.hpp"
#include <vector>


struct Node {
	Node();

	Edge * from_edge = nullptr;
	std::vector<Edge> to_edges;

	int get_material(Chess_Board::Piece_color color) const;
	void set_material(Chess_Board::Piece_color color, int material_value);

	private:
		int white_material = -INT32_MAX, black_material = -INT32_MAX;
};

struct Edge {
	Edge(const Move & mm, const Node * s, const Node & dd);
	const Node * src  = nullptr;
	Node dest;
	Move move;
};


#endif