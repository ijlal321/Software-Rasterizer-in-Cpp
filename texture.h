#pragma once
#include <cstdint>
#include <vector>

class tex2_t
{	
public:
	float u;
	float v;

	//tex2_t() : u(0.0f), v(0.0f) {};

	int texture_width{64};
	int texture_height{64};
	const uint32_t* mesh_texture;

	static const uint8_t REDBRICK_TEXTURE[];


};
