#include "display.h"


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

