#include <iostream>
#include <vector>
#include <SDL.h>
#include "display.h"
#include "vector.h"
#include "Mesh.h"


triangle_t triangles_to_render[N_MESH_FACES];
Mesh cube_mesh;

bool is_running = false;
std::vector<uint32_t> color_buffer;
Display display (color_buffer);


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

	for (int i = 0; i < N_MESH_FACES; i++) {
		face_t mesh_face = cube_mesh.mesh_faces[i];
		
		vec3_t mesh_vertices[3];
		mesh_vertices[0] = cube_mesh.mesh_vertices[mesh_face.a - 1];
		mesh_vertices[1] = cube_mesh.mesh_vertices[mesh_face.b - 1];
		mesh_vertices[2] = cube_mesh.mesh_vertices[mesh_face.c - 1];

		triangle_t projected_triangle;

		for (int j = 0; j < 3; j++) {
			vec3_t transformed_vertex = mesh_vertices[j];

			// Apply rotation
			transformed_vertex.vec3_rotate_x(cube_rotation.x);
			transformed_vertex.vec3_rotate_y(cube_rotation.y);
			transformed_vertex.vec3_rotate_z(cube_rotation.z);

			// Translate the vertex away from the camera
			transformed_vertex.z -= camera_position.z;

			// Project the current vertex
			vec2_t projected_point = project(transformed_vertex);

			// Scale and translate the projected points to the middle of the screen
			projected_point.x += (display.window_width / 2);
			projected_point.y += (display.window_height / 2);

			projected_triangle.points[j] = projected_point;

		}

		// Save the projected triangle in the array of triangles to render
		triangles_to_render[i] = projected_triangle;

	}

}

void render() {

	display.draw_grid(0xFF444444);

	// Loop all projected triangles and render them
	for (int i = 0; i < N_MESH_FACES; i++) {
		triangle_t triangle = triangles_to_render[i];
		display.draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
		display.draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
		display.draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
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