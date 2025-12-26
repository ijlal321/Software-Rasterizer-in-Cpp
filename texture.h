#pragma once
#include <cstdint>
#include <vector>

class tex2_t
{	
public:
	float u;
	float v;

	//tex2_t() : u(0.0f), v(0.0f) {};


	int texture_width;
	int texture_height;
	std::vector<uint32_t> mesh_texture;

	void load_redbrick_texture();
	
	static const int REDBRICK_TEXTURE_WIDTH {64};
	static const int REDBRICK_TEXTURE_HEIGHT {64};
	static const std::vector<uint8_t> REDBRICK_TEXTURE;

};

