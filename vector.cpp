#include "vector.h"
#include "math.h"

void vec3_t::vec3_rotate_x(float angle) {
	float ty = y * cos(angle) - z * sin(angle);
	z = y * sin(angle) + z * cos(angle);
	y = ty;
}

void vec3_t::vec3_rotate_y(float angle) {
    float tx = x * cos(angle) - z * sin(angle);
    z = x * sin(angle) + z * cos(angle);
	x = tx;
}

void vec3_t::vec3_rotate_z(float angle) {
    float tx = x * cos(angle) - y * sin(angle);
    y = x * sin(angle) + y * cos(angle);
	x = tx;
}
