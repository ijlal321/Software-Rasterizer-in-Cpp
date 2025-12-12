#include <iostream>
#include <vector>
#include <SDL.h>

bool is_running = false;

SDL_Window * window = nullptr;
SDL_Renderer* renderer = nullptr;

std::vector<uint32_t> color_buffer;
SDL_Texture* color_buffer_texture = nullptr;

int window_width = 800;
int window_height = 600;

bool initialize_window() {
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

void setup() {
	color_buffer.resize(window_width * window_height);

	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
}

void process_input() {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			is_running = false;
		}
		break;
	default:
		break;
	}
}

void update(void) {
	// TODO:
}

void render_color_buffer() {
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

void draw_grid() {
	int multiple = 10;
	for (int r = 0; r < window_height; r += multiple) {
		for (int c = 0; c < window_width; c++) {
			color_buffer[(window_width * r) + c] = 0xFF000000;
		}
	}
	
	for (int c = 0; c < window_width; c += multiple) {
		for (int r = 0; r < window_height; r++) {
			color_buffer[(window_width * r) + c] = 0xFF000000;
		}
	}
}

void draw_rect(int x_pos, int y_pos, int height, int width, uint32_t color) {
	for (int r = y_pos; r < y_pos + height; r++) {
		for (int c = x_pos; c < x_pos + width; c++) {
			color_buffer[(r * window_width) + c] = color;
		}
	}
}

void clear_color_buffer(uint32_t color) {
	for (int y = 0; y < window_height; y++) {
		for (int x = 0; x < window_width; x++) {
			color_buffer[(window_width * y) + x] = color;
		}
	}
}

void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	draw_grid();
	draw_rect(100, 500, 200, 100, 0xFFFF0000);

	render_color_buffer();
	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);
}

void destroy_window() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(color_buffer_texture);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* args[]) {
	is_running = initialize_window();

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}