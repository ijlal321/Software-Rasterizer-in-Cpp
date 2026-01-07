#pragma once

#include <SDL.h>
#include <iostream>
#include <vector>
#include "texture.h"

#define FPS 30 
#define FRAME_TARGET_TIME (1000 / FPS)


enum class Cull_Method {
	CULL_NONE,
	CULL_BACKFACE
};

enum class Render_Method {
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_FILL_TRIANGLE,
	RENDER_FILL_TRIANGLE_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE
};

class Display
{
public:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	std::vector<uint32_t>& color_buffer;
	std::vector<float>& z_buffer;

	SDL_Texture* color_buffer_texture = nullptr;

	Cull_Method cull_method = Cull_Method::CULL_BACKFACE;
	Render_Method render_method = Render_Method::RENDER_FILL_TRIANGLE_WIRE;

	// constructors
	Display(std::vector<uint32_t>& color_buffer, std::vector<float>& zb) : color_buffer(color_buffer),
		z_buffer(zb) {};

	Display() = delete;


	int window_width = 800;
	int window_height = 600;


	bool initialize_window();
	void draw_grid(uint32_t color);
	void draw_rect(int x_pos, int y_pos, int height, int width, uint32_t color);
	void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
	void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
	void render_color_buffer();
	void clear_color_buffer(uint32_t color);
	void destroy_window();
	void setup();
	void render();
	void draw_pixel(int x, int y, uint32_t color);
	void clear_z_buffer();
};

