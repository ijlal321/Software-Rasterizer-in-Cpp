#pragma once
class vec2_t
{
public:
	float x, y;

	vec2_t(float _x = 0.f, float _y = 0.f) : x(_x), y(_y) {};

	int vec2_length();
	// getting Unit Vector [just direction] = Better Optomization
	void vec2_normalize();


	static vec2_t vec2_add(vec2_t a, vec2_t b);
	static vec2_t vec2_sub(vec2_t a, vec2_t b);
	static vec2_t vec2_mul(vec2_t v, float factor);
	static vec2_t vec2_div(vec2_t v, float factor);
	static float vec2_dot(vec2_t a, vec2_t b);
};

class vec3_t
{
public:
	float x, y,z;
	
	vec3_t(float _x = 0.f, float _y = 0.f, float _z = 0.f) : x(_x), y(_y), z(_z) {};

	void vec3_rotate_x(float angle);
	void vec3_rotate_y(float angle);
	void vec3_rotate_z(float angle);

	float vec3_length();
	// getting Unit Vector [just direction] = Better Optomization
	void vec3_normalize();
	
	static vec3_t vec3_add(vec3_t a, vec3_t b);
	static vec3_t vec3_sub(vec3_t a, vec3_t b);
	static vec3_t vec3_mul(vec3_t v, float factor);
	static vec3_t vec3_div(vec3_t v, float factor);
	static vec3_t vec3_cross(vec3_t a, vec3_t b);
	static float vec3_dot(vec3_t a, vec3_t b);



};


