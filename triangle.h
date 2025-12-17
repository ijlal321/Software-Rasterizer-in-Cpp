#pragma once
#include "vector.h"
#include "display.h"

class face_t
{
public:
	int a, b, c;
	uint32_t color;
	face_t(int _a, int _b, int _c, uint32_t _color) : a(_a), b(_b), c(_c), color(_color) {}
	face_t() : a(0), b(0), c(0), color(0xFFFFFFFF) {};
};

class triangle_t
{
public:
	vec2_t points[3];
	uint32_t color;
	float avg_depth;

	static void draw_filled_triangle(Display& display, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

};



