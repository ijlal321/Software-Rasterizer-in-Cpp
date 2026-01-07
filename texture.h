#pragma once
#include <cstdint>
#include <vector>
#include "upng.h"

class tex2_t
{	
public:
	float u;
	float v;

	//tex2_t() : u(0.0f), v(0.0f) {};

};

class texture_t {
public:
	int texture_width{ 64 };
	int texture_height{ 64 };
	const uint32_t* mesh_texture;
	upng_t* png_texture = NULL;

	static const uint8_t REDBRICK_TEXTURE[];


	void load_png_texture_data(const char* filename);
};