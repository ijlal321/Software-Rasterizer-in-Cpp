#include "display.h"

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


void Display::draw_simple_texture(tex2_t& texture, int x, int y) {
	int scale_x = 1, scale_y = 1;
	for (int tex_y = 0; tex_y < texture.texture_height; tex_y++) {
		for (int tex_x = 0; tex_x < texture.texture_width; tex_x++) {
			uint32_t color = texture.mesh_texture[(texture.texture_width * tex_y) + tex_x];
			draw_rect(x + tex_x * scale_x, y + tex_y * scale_y, scale_y, scale_x, color);
		}
	}
}


void Display::draw_textured_triangle(tex2_t& texture, int x0, int y0, int x1, int y1, int x2, int y2,
	int u0, int v0, int u1, int v1, int u2, int v2) {
	
	// 0. normalize u,v which are just 0 and 1 now.
	u0 *= texture.texture_width;
	u1 *= texture.texture_width;
	u2 *= texture.texture_width;
	v0 *= texture.texture_height;
	v1 *= texture.texture_height;
	v2 *= texture.texture_height;



	// 1. Sort vertices by y-coordinate ascending (y0 <= y1 <= y2)
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

	// ISSUE IN GIGHUB ONE HERE, LAST ONE HE DOES WITH Y2, BUT Y2 HAS CHANGED. IT SHOULD BE Y1.


	if (y1 == y2) {
		// Draw flat-bottom triangle
		rasterize_textured_flat_bottom_triangle(texture, this, x0, y0, x1, y1, x2, y2,
			u0, v0, u1, v1, u2, v2);
	}
	else if (y0 == y1) {
		// Draw flat-top triangle
		//fill_flat_top_triangle(display, x0, y0, x1, y1, x2, y2, color);
	}
	else {
		// 2. Compute Middle Points
		// Calculate the new vertex (Mx,My) using triangle similarity
		int My = y1;
		int Mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;

		int Mv = v1;
		//int Mu = (((u2 - u0) * (v1 - v0)) / (v2 - v0)) + u0;
		int Mu = ((u2 + u0) / 2);

		// 3. Rasterize Top Half (v0, v1, M) [Flat bottom]
		rasterize_textured_flat_bottom_triangle(texture, this, x0, y0, x1, y1, Mx, My,
			u0, v0, u1, v1, Mu, Mv);
	}

}


// BIRTH OF SCALING ALGORITHM FOR TEXTURE MAPPING ... WOOHOOO
void rasterize_textured_flat_bottom_triangle(tex2_t& texture, Display* display,
	int x0, int y0, int x1, int y1, int x2, int y2,
	int u0, int v0, int u1, int v1, int u2, int v2) {

	int y_inc = 1; // we move one pixel down each step
	int nr_y_steps = y1 - y0; // total number of steps we take down
	float l_x_inc_each_y = (float)(x1 - x0) / nr_y_steps; // how much to increment left x each step down
	float r_x_inc_each_y = (float)(x2 - x0) / nr_y_steps; // how much to increment right x each step down

	float t_y_inc = (float)(v1 - v0) / nr_y_steps; // how much to increment texture v coord each step down
	float t_x_l_inc = (float)(u1 - u0) / nr_y_steps; // how much to increment texture u coord left side each step down
	float t_x_r_inc = (float)(u2 - u0) / nr_y_steps; // how much to increment texture u coord right side each step down

	// Start x_start and x_end from the top vertex (x0,y0)
	float x_start = x0;
	float x_end = x0;

	float t_y = v0;
	float t_x_start = u0;
	float t_x_end = u0;


	// Loop all the scanlines from top to bottom
	for (int y = y0; y < y2; y++) {
		if (round(t_y) == 0)
			continue;
			
		// print all information being passed to function
		//std::cout << "Drawing scanline at y=" << y << " from x_start=" << round(x_start) << " to x_end=" << round(x_end)
		//	<< " with texture coords u_start=" << round(t_x_start) << ", u_end=" << round(t_x_end) << ", v=" << round(t_y) << std::endl;

		draw_straight_textured_line(texture, display,
			round(x_start), round(x_end), y,
			round(t_x_start), round(t_x_end), round(t_y));
		x_start += l_x_inc_each_y;
		x_end += r_x_inc_each_y;

		t_y += t_y_inc;
		t_x_start += t_x_l_inc;
		t_x_end += t_x_r_inc;
	}


	return;
}

void draw_straight_textured_line(tex2_t& texture, Display* display,
	int x0, int x1, int y,
	int u0, int u1, int v) {

	// print u0 u1 and v
    //std::cout << "u0: " << u0 << ", u1: " << u1 << ", v: " << v << std::endl;

	int left_x, right_x;
	int left_u, right_u;

	if (x0 < x1) {
		left_x = x0;
		right_x = x1;
		left_u = u0;
		right_u = u1;
	}
	else {
		left_x = x1;
		right_x = x0;
		left_u = u1;
		right_u = u0;
	}
	if (right_x == left_x) {
		return; // avoid division by zero
	}

	int delta_x = right_x - left_x; // nr of steps we take horizontally [given inc of each step is 1]

	float u_inc = (float)(right_u - left_u) / delta_x; // how much to increment texture u coord each step right

	// print all information
	//std::cout << "Drawing textured line from (" << left_x << "," << y << ") to (" << right_x << "," << y << ") with texture u from " << left_u << " to " << right_u << " at v=" << v << " with delta_x: " << delta_x << " incr_u: " << u_inc   << std::endl;
	//
	//std::cin.get();
	int current_u = left_u;
	//std::cout << texture.mesh_texture[64 * 64];
	for (int x = 0; x < delta_x; x++) {
		// Get texture color from (current_u, v)
		uint32_t color = texture.mesh_texture[(texture.texture_width * (64-v)) + (current_u)];
		display->draw_pixel(left_x + x, y, color);
	
		current_u = left_u + (u_inc * x);
	}
}