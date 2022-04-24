#pragma once


enum CommandType { GET_POSSIBLE, SET_FROM, SET_TO, AI, RESET, GET_ALL_PIECES };

void send_restart_command();
void send_move_to_algorithm(int fx, int fy, int tx, int ty);
void get_possible_moves(byte * arr, int x, int y);
void get_all_pieces(byte * arr);
void get_ai_move_from_algorithm(int * fx, int * fy, int * tx, int * ty);