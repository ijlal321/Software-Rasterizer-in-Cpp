#pragma once
#include <vector>
#include "vector.h"


class light_t {
public:
	vec3_t direction;
	light_t(vec3_t dir = vec3_t(0.f, -1.f, 0.f)) : direction(dir) {};

	static uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor);
};
