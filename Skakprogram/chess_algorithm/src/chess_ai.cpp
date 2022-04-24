#include "../include/chess_ai.hpp"
#include "../include/digraph.hpp"
#include "../../chess_lib/include/point.hpp"
#include "../../chess_lib/include/defines.hpp"

int piece_values[B_KING + 1] = {
	0, // NO_PIECE	
	1, // W_PAWN	
	3, // W_KNIGHT	
	3, // W_BISHOP	
	5, // W_ROOK	
	9, // W_QUEEN	
	100, // W_KING	
	1, // B_PAWN	
	3, // B_KNIGHT	
	3, // B_BISHOP	
	5, // B_ROOK	
	9, // B_QUEEN	
	100  // B_KING	
};

int get_piece_value(char piece_id) {
	return piece_values[piece_id];
}

/*
	Get material value of all pieces belonging to the specified team.
*/
int get_color_material(const Chess_Board & board, Chess_Board::Piece_color color) {
	int material_value = 0;
	for (char y = 0; y < BOARD_HEIGHT; y++) 
		for (char x = 0; x < BOARD_WIDTH; x++) {
			Point pos {x, y}; 
			
			if (board.color_of_piece(pos) != color)
				continue;
			material_value += get_piece_value(board.get_piece_id({x, y}));
		}
	return material_value;
}

Chess_Board::Piece_color other_color(Chess_Board::Piece_color color) {
	return (color == Chess_Board::Piece_color::BLACK) ? Chess_Board::Piece_color::WHITE : Chess_Board::Piece_color::BLACK;
}

Move Chess_AI::get_best_move(Chess_Board & board) {
	// initialize the start row, which will, in turn, create the rest of the digraph.
	Node start_node;
	initialize_node(&start_node, board, 0);

	float highest_material_diff = -INT32_MAX;
	int best_index = -1;
	
	for (int i = 0; i < start_node.to_edges.size(); i++) {
		Node child_node = start_node.to_edges[i].dest;

		float child_material_diff = get_children_average_material_difference(&child_node, 0);

		if (child_material_diff > highest_material_diff) {
			highest_material_diff = child_material_diff;
			best_index = i;
		}
	}

	return start_node.to_edges[best_index].move;
}

std::vector<Move> get_possible_moves(const Chess_Board & board, const Point & p) {
	std::vector<Move> result;
	for (char y = 0; y < BOARD_HEIGHT; y++)
		for (char x = 0; x < BOARD_WIDTH; x++) {
			Move new_move {board, p, {x, y}};
			if (new_move.is_valid) 
				result.push_back(new_move);
		}
	return result;
}


void push_new_child_edge_from_move(const Move & m, Node * source_node) {
	// push a new edge, initiated with this as the parent:
	// 	a new node as the destination, and the current move as the move. 
	source_node->to_edges.push_back({m, source_node, Node {}}); 

	Edge * created_edge_ptr 	 = &source_node->to_edges.back();
	Node * destination_node_ptr = &created_edge_ptr->dest;

	// set the destination node to have this new edge as its "from_edge"
	destination_node_ptr->from_edge = created_edge_ptr;
}

void Chess_AI::initialize_node(Node * node, Chess_Board & board, int depth) {
	// get the move, that got us to this node, the move corresponding to the previous edge
	Move * new_move = nullptr;
	if (node->from_edge != nullptr) {
		new_move = &node->from_edge->move;

		// do the move.
		m_taken_pieces.push_back(board.do_move(*new_move));
	}

	// set the nodes material value based on the leading edges move -> the current board
	int white_mat = get_color_material(board, Chess_Board::Piece_color::WHITE);
	int black_mat = get_color_material(board, Chess_Board::Piece_color::BLACK);
	node->set_material(Chess_Board::Piece_color::WHITE, white_mat);
	node->set_material(Chess_Board::Piece_color::BLACK, black_mat);

	// we have gone to the bottom of the wanted depth, we will not initialize edges
	if (depth >= search_depth) {
		node->to_edges = {};
	} else {
		// loop through all points
		for (char y = 0; y < BOARD_HEIGHT; y++) {
			for (char x = 0; x < BOARD_WIDTH; x++) {
				Point p {x, y};

				// make sure we are on the right piece color
				Chess_Board::Piece_color turn = (depth % 2 == 0) ? m_color : other_color(m_color);
				if (board.color_of_piece(p) != turn)
					continue;

				// get every possible move from the position
				std::vector<Move> all_moves = get_possible_moves(board, p);

				// for every move, create a new edge
				for (Move & m : all_moves) {
					// the given move m is used to instanciate and push a new edge
					// to the to_edges vector
					push_new_child_edge_from_move(m, node);

					Edge * new_edge = &node->to_edges.back();

					// initialize the destination of the new edge, providing depth + 1
					initialize_node(&new_edge->dest, board, depth + 1);

					// checks, whether the child node was given a material value for each color
					if (new_edge->dest.get_material(Chess_Board::Piece_color::WHITE) < -1000 
					 || new_edge->dest.get_material(Chess_Board::Piece_color::BLACK) < -1000) {
						printf("Error: No board value given to child. Depth: %d\n", depth);
						exit(1);
					}
				}
			}
		}
	}

	// undo the move.
	if (new_move != nullptr) {
		board.undo_move(*new_move, m_taken_pieces.back());
		m_taken_pieces.pop_back();
	}
}

float Chess_AI::get_children_average_material_difference(Node * node, int depth) {
	int num_children = node->to_edges.size();
	//TODO: CRINGE, DOESNT WORK
	if (num_children == 0) {
		return node->get_material(m_color);// - node->get_material(other_color(m_color));
	}

	float sum = 0;

	for (Edge & e : node->to_edges) {
		sum += get_children_average_material_difference(&e.dest, depth + 1);
	}

	// std::cout << sum << '\n';

	return sum / (float) num_children;
}