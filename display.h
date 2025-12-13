#pragma once

#include <SDL.h>
#include <iostream>
#include <vector>

#define FPS 30 
#define FRAME_TARGET_TIME (1000 / FPS)

class Display
{
public:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	std::vector<uint32_t>& color_buffer;
	SDL_Texture* color_buffer_texture = nullptr;

	// constructors
	Display(std::vector<uint32_t>& color_buffer) : color_buffer(color_buffer) {};


	int window_width = 800;
	int window_height = 600;


	bool initialize_window();
	void draw_grid(uint32_t color);
	void draw_rect(int x_pos, int y_pos, int height, int width, uint32_t color);
	void render_color_buffer();
	void clear_color_buffer(uint32_t color);
	void destroy_window();
	void setup();
	void render();
	void draw_pixel(int x, int y, uint32_t color);
};

