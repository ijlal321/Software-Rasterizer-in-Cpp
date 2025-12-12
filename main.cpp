#include <iostream>
#include <vector>
#include <SDL.h>
#include "display.h"

bool is_running = false;
std::vector<uint32_t> color_buffer;
Display display (color_buffer);

void process_input() {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		std::cout << "pressed" << std::endl;
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

void render() {

	display.draw_grid(0xFFFFFF00);
	display.draw_rect(100, 500, 200, 100, 0xFFFF0000);

	display.render();
}


int main(int argc, char* args[]) {
	is_running = display.initialize_window();

	display.setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	display.destroy_window();

	return 0;
}