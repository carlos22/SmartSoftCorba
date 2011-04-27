//--------------------------------------------------------------------------
//  Copyright (C) 2009 Manuel Wopfner
//
//        wopfner@hs-ulm.de
//
//--------------------------------------------------------------------------

#ifndef STRUCTS_HH_
#define STRUCTS_HH_

struct Point {
	float x;
	float y;
	float z;
	float r; // color
	float g; // color
	float b; // color

	Point(float x, float y, float z, float red = 1.0, float green = 0.0, float blue = 0.0) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = red;
		this->g = green;
		this->b = blue;
	}
};

struct Line {
	Point begin;
	Point end;

	Line(Point begin, Point end) :
		begin(begin), end(end) {
	}
};

struct Cube {
	float x;
	float y;
	float z;
	float r; // color
	float g; // color
	float b; // color
	float size;

	Cube(float x, float y, float z, float size, float red = 0.0, float green = 1.0, float blue =
			0.0) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = red;
		this->g = green;
		this->b = blue;
		this->size = size;
	}
};

#endif /* STRUCTS_H_ */
