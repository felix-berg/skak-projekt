#include "../include/digraph.hpp"

Edge::Edge(const Move & mm, const Node * ss, const Node & dd) 
	: move {mm}, src { ss }, dest { dd }
{ }

Node::Node() 
{ }

int Node::get_material(Chess_Board::Piece_color color) const { 
	if (color == Chess_Board::Piece_color::WHITE)
		return white_material;
	else
		return black_material;
}

void Node::set_material(Chess_Board::Piece_color color, int material_value) { 
	if (color == Chess_Board::Piece_color::WHITE)
		white_material = material_value;
	else
		black_material = material_value;
}
