#include "../include/point.hpp"

void Point::get_name(char result[2]) const {
	result[0] = 'A' + x;
	result[1] = '1' + y;
}

/*
	Get a point object from given formal chess name e.g. "E4".
	Does minimal error-checking
*/
Point point_from_name(const char name[2]) {
	return Point {
		(char) (name[0] - 'A'),
		(char) (name[1] - '1')
	};
}

bool operator == (const Point & p1, const Point & p2) {
	return p1.x == p2.x && 
			 p1.y == p2.y;
}

bool operator != (const Point & p1, const Point & p2) {
	return !(p1 == p2);
}
