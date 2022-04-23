#ifndef POINT_HPP
#define POINT_HPP

class Point {
	public:
		char x, y;
		Point(char xx, char yy) : x { xx }, y { yy } { }; 
		Point() : x { 0 }, y { 0 } { };
		void get_name(char * res) const;
};

bool operator == (const Point & p1, const Point & p2);
bool operator != (const Point & p1, const Point & p2);

Point point_from_name(const char name[2]);

#endif