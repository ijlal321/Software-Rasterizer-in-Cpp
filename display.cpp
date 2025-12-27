#include "display.h"
#include "vector.h"

// =============================
// Prototypes for helper functions
// =============================

void rasterize_textured_flat_bottom_triangle(tex2_t& texture, Display* display,
	int x0, int y0, int x1, int y1, int x2, int y2,
	int u0, int v0, int u1, int v1, int u2, int v2);

void draw_straight_textured_line(tex2_t& texture, Display* display,
	int x0, int x1, int y,
	int u0, int u1, int v);

// =============================
// Display class function definitions
// =============================

bool Display::initialize_window() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	// get screen info
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;
	window_height = display_mode.h;
	std::cout << "Screen width: " << window_width << ", Screen height: " << window_height << std::endl;
	
	// create a window
	window = SDL_CreateWindow(
		"My window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
	);

	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	// create a renderer
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

void Display::draw_grid(uint32_t color) {
	int multiple = 10;
	for (int r = 0; r < window_height; r += multiple) {
		for (int c = 0; c < window_width; c += multiple) {
			color_buffer[(window_width * r) + c] = color;
		}
	}

	//for (int c = 0; c < window_width; c += multiple) {
	//	for (int r = 0; r < window_height; r++) {
	//		color_buffer[(window_width * r) + c] = color;
	//	}
	//}
}

void Display::draw_rect(int x_pos, int y_pos, int height, int width, uint32_t color) {
	for (int r = y_pos; r < y_pos + height; r++) {
		for (int c = x_pos; c < x_pos + width; c++) {
			/*color_buffer[(r * window_width) + c] = color;*/
			draw_pixel(c, r, color);
		}
	}
}

void Display::draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int longest_side_length = abs(delta_x) > abs(delta_y) ? abs(delta_x) : abs(delta_y);

	// Find how much we should increment in both x and y each step
	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i <= longest_side_length; i++) {
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}

void Display::draw_triangle (int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}

void Display::clear_color_buffer(uint32_t color) {
	for (int y = 0; y < window_height; y++) {
		for (int x = 0; x < window_width; x++) {
			color_buffer[(window_width * y) + x] = color;
		}
	}
}

void Display::destroy_window() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(color_buffer_texture);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Display::render_color_buffer() {
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer.data(),
		(int)(window_width * sizeof(uint32_t))
	);

	SDL_RenderCopy(
		renderer,
		color_buffer_texture,
		NULL,
		NULL
	);
}

void Display::setup() {
	color_buffer.resize(window_width * window_height);

	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
}

void Display::render() {
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//SDL_RenderClear(renderer);

	render_color_buffer();
	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);
}

void Display::draw_pixel(int x, int y, uint32_t color) {
	if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
		return;
	}
	color_buffer[(window_width * y) + x] = color;
}


void Display::draw_textured_triangle(
	int x0, int y0, float u0, float v0,
	int x1, int y1, float u1, float v1,
	int x2, int y2, float u2, float v2,
	tex2_t& texture
) {
	// We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
	if (y0 > y1) {
		std::swap(y0, y1);
		std::swap(x0, x1);
		std::swap(u0, u1);
		std::swap(v0, v1);
	}
	if (y1 > y2) {
		std::swap(y1, y2);
		std::swap(x1, x2);
		std::swap(u1, u2);
		std::swap(v1, v2);
	}
	if (y0 > y1) {
		std::swap(y0, y1);
		std::swap(x0, x1);
		std::swap(u0, u1);
		std::swap(v0, v1);
	}

	// Create vector points and texture coords after we sort the vertices
	vec2_t point_a ( x0, y0 );
	vec2_t point_b ( x1, y1 );
	vec2_t point_c ( x2, y2 );

	///////////////////////////////////////////////////////
	// Render the upper part of the triangle (flat-bottom)
	///////////////////////////////////////////////////////
	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 - y0 != 0) {
		float tex_y_inc = (v2 - v0) / (y2 - y0);
		for (int y = y0; y <= y1; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;
			int tex_y = (v0 + (y - y0) * tex_y_inc) * texture.texture_width;
			// texy 0-63 in loop. never 64. works good.
			std::cout << tex_y << std::endl;
				
			if (x_end < x_start) {
				std::swap(x_start, x_end); // swap if x_start is to the right of x_end
			}

			//std::cout << "x0 ->" << x0 << "x1 ->" << x1 << ", x2-> " << x2 << std::endl;

			for (int x = x_start; x < x_end; x++) { 
				float x_per = (x - x1) / (x2 - x1);
				//std::cout << x_per << std::endl;
				int tex_x = (u2 > u1 ? (u1 + (u2 - u1)) : (u2 + (u1 - u2))) * x_per;
				//std::cout << tex_x << std::endl;
				//uint32_t color = texture.mesh_texture[(texture.texture_width * tex_y) + tex_x];
				// Draw our pixel with the color that comes from the texture
				//draw_pixel(x, y, color); // debug checkerboard
				//draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
			}
			//std::cout << std::endl;
		}
	}

	///////////////////////////////////////////////////////
	// Render the bottom part of the triangle (flat-top)
	///////////////////////////////////////////////////////
	//inv_slope_1 = 0;
	//inv_slope_2 = 0;

	//if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	//if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	//if (y2 - y1 != 0) {
	//	for (int y = y1; y <= y2; y++) {
	//		int x_start = x1 + (y - y1) * inv_slope_1;
	//		int x_end = x0 + (y - y0) * inv_slope_2;

	//		if (x_end < x_start) {
	//			int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
	//		}

	//		for (int x = x_start; x < x_end; x++) {
	//			// Draw our pixel with the color that comes from the texture
	//			draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
	//		}
	//	}
	//}
}