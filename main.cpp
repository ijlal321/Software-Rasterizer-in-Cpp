#include <iostream>
#include <vector>
#include <SDL.h>
#include "display.h"
#include "vector.h"
#include "Mesh.h"


std::vector<triangle_t> triangles_to_render; // triangles given to 
mesh_t cube_mesh;  // Main Object we Display

bool is_running = false;
std::vector<uint32_t> color_buffer;
Display display (color_buffer);

vec3_t camera_position = { 0, 0, 0 };

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
		if (event.key.keysym.sym == SDLK_ESCAPE)
			is_running = false;
		if (event.key.keysym.sym == SDLK_1)
			display.render_method = Render_Method::RENDER_WIRE_VERTEX;
		if (event.key.keysym.sym == SDLK_2)
			display.render_method = Render_Method::RENDER_WIRE;
		if (event.key.keysym.sym == SDLK_3)
			display.render_method = Render_Method::RENDER_FILL_TRIANGLE;
		if (event.key.keysym.sym == SDLK_4)
			display.render_method = Render_Method::RENDER_FILL_TRIANGLE_WIRE;
		if (event.key.keysym.sym == SDLK_c)
			display.cull_method = Cull_Method::CULL_BACKFACE;
		if (event.key.keysym.sym == SDLK_d)
			display.cull_method = Cull_Method::CULL_NONE;
		break;	
	default:
		break;
	}
}

void setup() {
	// Initialize render mode and triangle culling method
	display.render_method = Render_Method::RENDER_WIRE;
	display.cull_method = Cull_Method::CULL_BACKFACE;

	display.setup();
	//cube_mesh.load_cube_mesh_data();
	cube_mesh.load_obj_file_data("assets/cube.obj");
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

	cube_mesh.rotation.x += 0.01f;
	cube_mesh.rotation.y += 0.01f;
	cube_mesh.rotation.z += 0.01f;

	/*
		1. Get each face
		2. get 3 vertices [3d] of a face.
		3. rotate vertices [3d] (according to rotation)
		3.5. move object back away from camera.
		4. check back face culling of face
		5. Project to 2d.
		6. store in triangles to renfer

	*/
	for (int i = 0; i < cube_mesh.faces.size(); i++) {
		face_t mesh_face = cube_mesh.faces[i]; // get face [3d] from mesh

		vec3_t mesh_vertices[3]; // store vertexes of face
		mesh_vertices[0] = cube_mesh.vertices[mesh_face.a - 1];
		mesh_vertices[1] = cube_mesh.vertices[mesh_face.b - 1];
		mesh_vertices[2] = cube_mesh.vertices[mesh_face.c - 1];

		// Loop all three vertices of this current face and apply transformations
		vec3_t transformed_vertices[3];
		for (int j = 0; j < 3; j++) {
			vec3_t transformed_vertex = mesh_vertices[j];

			// Apply rotation
			transformed_vertex.vec3_rotate_x(cube_mesh.rotation.x);
			transformed_vertex.vec3_rotate_y(cube_mesh.rotation.y);
			transformed_vertex.vec3_rotate_z(cube_mesh.rotation.z);

			// Translate the vertex away from the camera
			transformed_vertex.z += 5;

			transformed_vertices[j] = transformed_vertex;
		}

		// Backface culling test to see if the current face should be projected
		if (display.cull_method == Cull_Method::CULL_BACKFACE) {
			vec3_t vector_a = transformed_vertices[0]; /*   A   */
			vec3_t vector_b = transformed_vertices[1]; /*  / \  */
			vec3_t vector_c = transformed_vertices[2]; /* C---B */

			// Get the vector subtraction of B-A and C-A
			vec3_t vector_ab = vec3_t::vec3_sub(vector_b, vector_a);
			vec3_t vector_ac = vec3_t::vec3_sub(vector_c, vector_a);
			vector_ab.vec3_normalize();
			vector_ac.vec3_normalize();

			// Compute the face normal (using cross product to find perpendicular)
			vec3_t normal = vec3_t::vec3_cross(vector_ab, vector_ac);
			normal.vec3_normalize();

			// Find the vector between vertex A in the triangle and the camera origin
			vec3_t camera_ray = vec3_t::vec3_sub(camera_position, vector_a);

			// Calculate how aligned the camera ray is with the face normal (using dot product)
			float dot_normal_camera = vec3_t::vec3_dot(normal, camera_ray);

			// Bypass the triangles that are looking away from the camera
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		triangle_t projected_triangle; // project face [3D] to triangle [2D] and store it here

		for (int j = 0; j < 3; j++) {
			// Project the current vertex
			vec2_t projected_point = project(transformed_vertices[j]);

			// Scale and translate the projected points to the middle of the screen
			projected_point.x += (display.window_width / 2);
			projected_point.y += (display.window_height / 2);

			projected_triangle.points[j] = projected_point;

		}

		// Save the projected triangle in the array of triangles to render
		triangles_to_render.push_back(projected_triangle);

	}

}

void render() {

	display.draw_grid(0xFF444444);

	// Loop all projected triangles and render them
	for (int i = 0; i < triangles_to_render.size(); i++) {
		triangle_t triangle = triangles_to_render[i];
		//display.draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
		//display.draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
		//display.draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

		// Draw filled triangle
		if (display.render_method == Render_Method::RENDER_FILL_TRIANGLE || display.render_method == Render_Method::RENDER_FILL_TRIANGLE_WIRE) {
			triangle_t::draw_filled_triangle(display,
				triangle.points[0].x, triangle.points[0].y,
				triangle.points[1].x, triangle.points[1].y,
				triangle.points[2].x, triangle.points[2].y,
				0xFF555555
			);
		}

		// Draw triangle wireframe
		if (display.render_method == Render_Method::RENDER_WIRE || display.render_method == Render_Method::RENDER_WIRE_VERTEX || display.render_method == Render_Method::RENDER_FILL_TRIANGLE) {
			display.draw_triangle(
				triangle.points[0].x, triangle.points[0].y,
				triangle.points[1].x, triangle.points[1].y,
				triangle.points[2].x, triangle.points[2].y,
				0xFFFFFFFF
			);
		}

		// Draw triangle vertex points
		if (display.render_method == Render_Method::RENDER_WIRE_VERTEX) {
			display.draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000); // vertex A
			display.draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000); // vertex B
			display.draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000); // vertex C
		}

	}
	triangles_to_render.clear();
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