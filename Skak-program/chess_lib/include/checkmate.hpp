#define NOT_IN_CHECK 0
#define IN_CHECK 1
#define IN_CHECKMATE 2

class Chess_Board;
class Point;

#include <iostream>

int is_in_check(const Chess_Board & board, const Point & pos);
int is_in_checkmate(const Chess_Board & board, const Point & pos);