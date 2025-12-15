#pragma once

#include "vector.h"

class face_t
{
public:
	int a, b, c;
	face_t(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {}
};

class triangle_t
{
public:
	vec2_t points[3];

};
