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
	RENDER_FILL_TRIANGLE_WIRE
};

class Display
{
public:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	std::vector<uint32_t>& color_buffer;
	SDL_Texture* color_buffer_texture = nullptr;

	Cull_Method cull_method = Cull_Method::CULL_BACKFACE;
	Render_Method render_method = Render_Method::RENDER_FILL_TRIANGLE_WIRE;

	// constructors
	Display(std::vector<uint32_t>& color_buffer) : color_buffer(color_buffer) {};


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
	void draw_simple_texture(tex2_t& texture, int x, int y);
	void draw_textured_triangle(tex2_t& texture, int x0, int y0, int x1, int y1, int x2, int y2,
		int u0, int v0, int u1, int v1, int u2, int v2);
};

