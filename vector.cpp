#include "vector.h"
#include "math.h"

////////////////////////////////////////////////////////////////////////////////
// Vector 2D functions
////////////////////////////////////////////////////////////////////////////////

int vec2_t::vec2_length() {
	return sqrt(x * x + y * y);
}

void vec2_t::vec2_normalize() {
	float length = sqrt(x * x + y * y);
	x /= length;
	y /= length;
	// sqrt ( 25/34 + 9/34 ) = sqrt ( 34/34 ) = 1
}

////////////////////////////////////////////////////////////////////////////////
//		Vector 2D STATIC functions 


vec2_t vec2_t::vec2_add(vec2_t a, vec2_t b){
	vec2_t result = {
		a.x + b.x,
		a.y + b.y
	};
	return result;
}

vec2_t vec2_t::vec2_sub(vec2_t a, vec2_t b){
	vec2_t result = {
	a.x - b.x,
	a.y - b.y
	};
	return result;
}
vec2_t vec2_t::vec2_mul(vec2_t v, float factor){
	vec2_t result = {
	v.x * factor,
	v.y * factor
	};
	return result;
}
vec2_t vec2_t::vec2_div(vec2_t v, float factor) {
	vec2_t result = {
		v.x / factor,
		v.y / factor
	};
	return result;
}
float vec2_t::vec2_dot(vec2_t a, vec2_t b) {
	return (a.x * b.x) + (a.y * b.y);
}


////////////////////////////////////////////////////////////////////////////////
// Vector 3D functions
////////////////////////////////////////////////////////////////////////////////

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


float vec3_t::vec3_length() {
	return sqrt(x * x + y * y + z * z);
}
void vec3_t::vec3_normalize() {
	float length = sqrtf(x * x + y * y + z * z);
	x /= length;
	y /= length;
	z /= length;
}

vec3_t vec3_t::vec3_sub(vec3_t a, vec3_t b){
	vec3_t result = {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z
	};
	return result;
}
vec3_t vec3_t::vec3_add(vec3_t a, vec3_t b){
	vec3_t result = {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z
	};
	return result;
}
vec3_t vec3_t::vec3_mul(vec3_t v, float factor){
	vec3_t result = {
		v.x * factor,
		v.y * factor,
		v.z * factor
	};
	return result;
}
vec3_t vec3_t::vec3_div(vec3_t v, float factor){
	vec3_t result = {
		v.x / factor,
		v.y / factor,
		v.z / factor
	};
	return result;
}
vec3_t vec3_t::vec3_cross(vec3_t a, vec3_t b){
	vec3_t result = {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
	return result;
}
float vec3_t::vec3_dot(vec3_t a, vec3_t b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}


///////////////////////////////////////////////////////////////////////////////
// Implementations of Vector conversion functions
///////////////////////////////////////////////////////////////////////////////
vec4_t vec4_from_vec3(vec3_t v) {
	vec4_t result = { v.x, v.y, v.z, 1.0 };
	return result;
}

vec3_t vec3_from_vec4(vec4_t v) {
	vec3_t result = { v.x, v.y, v.z };
	return result;
}
