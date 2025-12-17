#pragma once
#include "vector.h"
#include "display.h"

class face_t
{
public:
	int a, b, c;
	face_t(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {}
	face_t() : a(0), b(0), c(0) {};
};

class triangle_t
{
public:
	vec2_t points[3];

	static void draw_filled_triangle(Display& display, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

};



