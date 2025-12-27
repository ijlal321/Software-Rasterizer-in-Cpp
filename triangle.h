#pragma once
#include "vector.h"
#include "display.h"
#include "texture.h"


class face_t
{
public:
	int a, b, c;
	tex2_t a_uv, b_uv, c_uv;
	uint32_t color;


	face_t(int _a, int _b, int _c, tex2_t _a_uv, tex2_t _b_uv, tex2_t _c_uv, uint32_t _color) :
		a(_a), b(_b), c(_c), a_uv(_a_uv), b_uv(_b_uv), c_uv(_c_uv), color(_color) {}
	//face_t(int _a, int _b, int _c, uint32_t _color) : a(_a), b(_b), c(_c), color(_color) {}
	face_t() : a(0), b(0), c(0), color(0xFFFFFFFF) {};
};

class triangle_t
{
public:
	vec2_t points[3];
	tex2_t texcoords[3];
	uint32_t color;
	float avg_depth;

	static void draw_filled_triangle(Display& display, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

	static void draw_textured_triangle(
		int x0, int y0, float u0, float v0,
		int x1, int y1, float u1, float v1,
		int x2, int y2, float u2, float v2,
		Display& display, tex2_t& texture
	);
};



