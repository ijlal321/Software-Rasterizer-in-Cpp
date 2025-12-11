#include <iostream>
#include <SDL.h>
SDL_Window * window = nullptr;
SDL_Renderer* renderer = nullptr;
bool is_running = false;

bool initialize_window() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	// create a window
	window = SDL_CreateWindow(
		"My window", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		800, 
		600, 
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

int main(int argc, char* args[]) {
	is_running = initialize_window();


	std::cin.get();
	return 0;
}