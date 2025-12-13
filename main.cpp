#include <iostream>
#include <vector>
#include <SDL.h>
#include "display.h"
#include "vector.h"

bool is_running = false;
std::vector<uint32_t> color_buffer;
Display display (color_buffer);

#define N_POINTS (9 * 9 * 9)
vec3_t cube_points[N_POINTS]; // 9x9x9 cube
vec2_t projected_points[N_POINTS];


vec3_t camera_position = { 0, 0, -5 };
vec3_t cube_rotation = { 0,0,0 };

float fov_factor = 640;
int previous_frame_time = 0;

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

void setup() {
	display.setup();

	int point_count = 0;

	// Start loading my array of vectors
	// From -1 to 1 (in this 9x9x9 cube)
	for (float x = -1; x <= 1; x += 0.25) {
		for (float y = -1; y <= 1; y += 0.25) {
			for (float z = -1; z <= 1; z += 0.25) {
				vec3_t new_point = { x, y, z };
				cube_points[point_count++] = new_point;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Function that receives a 3D vector and returns a projected 2D point
////////////////////////////////////////////////////////////////////////////////
vec2_t project(vec3_t point) {
	//std::cout << "x-> " << point.x << " y-> " << point.y << " z-> " << point.z << std::endl;
	vec2_t projected_point{
		(fov_factor * point.x) / point.z, 
		(fov_factor * point.y)/ point.z 
	};
	return projected_point;
}

void update(void) {
	// Wait some time until the reach the target frame time in milliseconds
	int wait_time = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	if (wait_time > 0 && wait_time <= FRAME_TARGET_TIME) {
		SDL_Delay(wait_time);
	}
	previous_frame_time = SDL_GetTicks();

	cube_rotation.x += 0.01f;
	cube_rotation.y += 0.01f;
	cube_rotation.z += 0.01f;

	for (int i = 0; i < N_POINTS; i++) {
		vec3_t point = cube_points[i];

		// Apply rotation
		point.vec3_rotate_x(cube_rotation.x);
		point.vec3_rotate_y(cube_rotation.y);
		point.vec3_rotate_z(cube_rotation.z);

		point.z -= camera_position.z; // Move the point relative to the camera position

		// Project the current point
		vec2_t projected_point = project(point);
		// Save the projected 2D vector in the array of projected points
		projected_points[i] = projected_point;
	}
}

void render() {

	display.draw_grid(0xFF444444);
	//display.draw_rect(100, 500, 200, 100, 0xFFFF0000);
	//display.draw_pixel(20, 20, 0xFF00FF00);


	// Loop all projected points and render them
	for (int i = 0; i < N_POINTS; i++) {
		vec2_t projected_point = projected_points[i];
		display.draw_rect(
			projected_point.x + (display.window_width / 2),
			projected_point.y + (display.window_height / 2),
			4,
			4,
			0xFFFFFF00
		);
	}



	display.render();
}




int main(int argc, char* args[]) {
	is_running = display.initialize_window();

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	display.destroy_window();

	return 0;
}