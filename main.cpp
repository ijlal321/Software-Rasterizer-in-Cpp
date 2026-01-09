#include <iostream>
#include <vector>
#include <SDL.h>
#include <algorithm>
#include <cstdint>
#include "display.h"
#include "vector.h"
#include "Mesh.h"
# include "matrix.h"
#include "light.h"
#include "texture.h"
#include "camera.h"
#include "clipping.h"
#include "object_manager.h"
#include <array>
	
const bool SIMULATE_LOW_RESOLUTION = false;
const int SIMULATED_WINDOW_WIDTH = 100 * 1.6; //  only applicable if simulating low res is true
const int SIMULATED_WINDOW_HEIGHT = 100;     // `` 

#define MAX_TRIANGLES 10000
std::array<triangle_t, MAX_TRIANGLES> triangles_to_render; // triangles given to 
int num_triangles_to_render = 0;

Object_Manager object_mgr;  // Main Object we Display
mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;

bool is_running = false;
std::vector<uint32_t> color_buffer;
std::vector<float> z_buffer;
Display display (color_buffer, z_buffer);

light_t main_light = { {0, 0, 1} };

frustrum_t view_frustrum;

camera_t camera {
	vec3_t{ 0, 0, 0 },
	vec3_t{ 0, 0, 1 },
	vec3_t{ 0, 0, 0 },
	0.0f
};

int previous_frame_time = 0;
float delta_time = 0;

void setup() {
	// Initialize render mode and triangle culling method
	display.render_method = Render_Method::RENDER_TEXTURED;
	display.cull_method = Cull_Method::CULL_BACKFACE;

	display.setup();

	// Initialize the perspective projection matrix
	float aspect_y = (float)display.window_height / (float)display.window_width;
	float aspect_x = (float)display.window_width / (float)display.window_height;
	float fov_y = 3.141592 / 3.0; // the same as 180/3, or 60deg
	float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
	float z_near = 1.0;
	float z_far = 20.0;
	proj_matrix = mat4_t::mat4_make_perspective(fov_y, aspect_y, z_near, z_far);

	// Initialize frustum planes with a point and a normal
	view_frustrum.init_frustum_planes(fov_x, fov_y, z_near, z_far);

	// Loads mesh entities
	object_mgr.load_object("./assets/runway.obj", "./assets/runway.png", vec3_t(1, 1, 1), vec3_t(0, -1.5, +23), vec3_t(0, 0, 0));
	object_mgr.load_object("./assets/f22.obj", "./assets/f22.png", vec3_t(1, 1, 1), vec3_t(0, -1.3, +5), vec3_t(0, -M_PI / 2, 0));
	object_mgr.load_object("./assets/efa.obj", "./assets/efa.png", vec3_t(1, 1, 1), vec3_t(-2, -1.3, +9), vec3_t(0, -M_PI / 2, 0));
	object_mgr.load_object("./assets/f117.obj", "./assets/f117.png", vec3_t(1, 1, 1), vec3_t(+2, -1.3, +9), vec3_t(0, -M_PI / 2, 0));
}


void process_input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {

		switch (event.type) {
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
				break;
			}
			if (event.key.keysym.sym == SDLK_1) {
				display.render_method = Render_Method::RENDER_WIRE_VERTEX;
				break;
			}
			if (event.key.keysym.sym == SDLK_2) {
				 display.render_method = Render_Method::RENDER_WIRE;
				 break;
			}
			if (event.key.keysym.sym == SDLK_3) {
				 display.render_method = Render_Method::RENDER_FILL_TRIANGLE;
				 break;
			}
			if (event.key.keysym.sym == SDLK_4) {
				display.render_method = Render_Method::RENDER_FILL_TRIANGLE_WIRE;
				break;
			}
			if (event.key.keysym.sym == SDLK_5) {
				display.render_method = Render_Method::RENDER_TEXTURED;
				break;
			}
			if (event.key.keysym.sym == SDLK_6) {
				display.render_method = Render_Method::RENDER_TEXTURED_WIRE;
				break;
			}
			if (event.key.keysym.sym == SDLK_c) {
				display.cull_method = Cull_Method::CULL_BACKFACE;
				break;
			}
			if (event.key.keysym.sym == SDLK_x) {
				display.cull_method = Cull_Method::CULL_NONE;
				break;
			}
			if (event.key.keysym.sym == SDLK_w) {
				camera.pitch += 3.0 * delta_time;
				break;
			}
			if (event.key.keysym.sym == SDLK_s) {
				camera.pitch -= 3.0 * delta_time;
				break;
			}
			if (event.key.keysym.sym == SDLK_RIGHT) {
				camera.yaw += 1.0 * delta_time;
				break;
			}
			if (event.key.keysym.sym == SDLK_LEFT) {
				camera.yaw -= 1.0 * delta_time;
				break;
			}
			if (event.key.keysym.sym == SDLK_UP) {
				camera.forward_velocity = vec3_t::vec3_mul(camera.direction, 5.0 * delta_time);
				camera.position = vec3_t::vec3_add(camera.position, camera.forward_velocity);
				break;
			}
			if (event.key.keysym.sym == SDLK_DOWN) {
				camera.forward_velocity = vec3_t::vec3_mul(camera.direction, 5.0 * delta_time);
				camera.position = vec3_t::vec3_sub(camera.position, camera.forward_velocity);
				break;
			}	

			if (event.key.keysym.sym == SDLK_a) {
				camera.position.y += 3.0 * delta_time;
				break;
			}
			if (event.key.keysym.sym == SDLK_d) {
				camera.position.y -= 3.0 * delta_time;
				break;
			}

			break;
		default:
			break;
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles
///////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
///////////////////////////////////////////////////////////////////////////////
void process_graphics_pipeline_stages(mesh_t& mesh_to_render, texture_t* texture_to_load) {
    // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_t::mat4_make_scale(mesh_to_render.scale.x, mesh_to_render.scale.y, mesh_to_render.scale.z);
	mat4_t translation_matrix = mat4_t::mat4_make_translation(mesh_to_render.translation.x, mesh_to_render.translation.y, mesh_to_render.translation.z);
	mat4_t rotation_matrix_x = mat4_t::mat4_make_rotation_x(mesh_to_render.rotation.x);
	mat4_t rotation_matrix_y = mat4_t::mat4_make_rotation_y(mesh_to_render.rotation.y);
	mat4_t rotation_matrix_z = mat4_t::mat4_make_rotation_z(mesh_to_render.rotation.z);

	// Update camera look at target to create view matrix
	vec3_t target = camera.get_camera_lookat_target();
	vec3_t up_direction{ 0, 1, 0 };
	view_matrix = mat4_t::mat4_look_at(camera.position, target, up_direction);

	// Loop all triangle faces of our mesh
	for (int i = 0; i < mesh_to_render.faces.size(); i++) {
		face_t mesh_face = mesh_to_render.faces[i]; // get face [3d] from mesh

		vec3_t face_vertices[3]; // store vertexes of face
		face_vertices[0] = mesh_to_render.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh_to_render.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh_to_render.vertices[mesh_face.c - 1];

		vec4_t transformed_vertices[3];

		// Loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

			// Create a World Matrix combining scale, rotation, and translation matrices
			world_matrix = mat4_t::mat4_identity();

			// Order matters: First scale, then rotate, then translate. [T]*[R]*[S]*v
			world_matrix = mat4_t::mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_t::mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_t::mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_t::mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_t::mat4_mul_mat4(translation_matrix, world_matrix);

			// Multiply the world matrix by the original vector
			transformed_vertex = mat4_t::mat4_mul_vec4(world_matrix, transformed_vertex);

			// Multiply the view matrix by the vector to transform the scene to camera space
			transformed_vertex = mat4_t::mat4_mul_vec4(view_matrix, transformed_vertex);


			// Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}

		// Get individual vectors from A, B, and C vertices to compute normal
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

		// Get the vector subtraction of B-A and C-A
		vec3_t vector_ab = vec3_t::vec3_sub(vector_b, vector_a);
		vec3_t vector_ac = vec3_t::vec3_sub(vector_c, vector_a);
		vector_ab.vec3_normalize();
		vector_ac.vec3_normalize();

		// Compute the face normal (using cross product to find perpendicular)
		vec3_t normal = vec3_t::vec3_cross(vector_ab, vector_ac);
		normal.vec3_normalize();

		// Find the vector between vertex A in the triangle and the camera origin
		vec3_t origin = { 0, 0, 0 };
		vec3_t camera_ray = vec3_t::vec3_sub(origin, vector_a);

		// Calculate how aligned the camera ray is with the face normal (using dot product)
		float dot_normal_camera = vec3_t::vec3_dot(normal, camera_ray);

		// Backface culling test to see if the current face should be projected
		if (display.cull_method == Cull_Method::CULL_BACKFACE) {

			// Bypass the triangles that are looking away from the camera
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		// Create a polygon from the original transformed triangle to be clipped
		polygon_t polygon = frustrum_t::polygon_from_triangle(
			vec3_from_vec4(transformed_vertices[0]),
			vec3_from_vec4(transformed_vertices[1]),
			vec3_from_vec4(transformed_vertices[2]),
			mesh_face.a_uv,
			mesh_face.b_uv,
			mesh_face.c_uv
		);

		// Clip the polygon and returns a new polygon with potential new vertices
		view_frustrum.clip_polygon(&polygon);

		// Break the clipped polygon apart back into individual triangles
		triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;

		frustrum_t::triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

		// Loops all the assembled triangles after clipping
		for (int t = 0; t < num_triangles_after_clipping; t++) {
			triangle_t triangle_after_clipping = triangles_after_clipping[t];

			vec4_t projected_points[3];

			// Loop all three vertices to perform projection
			for (int j = 0; j < 3; j++) {
				// Project the current vertex	
				projected_points[j] = mat4_t::mat4_mul_vec4(proj_matrix, triangle_after_clipping.points[j]);

				// Perform perspective divide
				if (projected_points[j].w != 0) {
					projected_points[j].x /= projected_points[j].w;
					projected_points[j].y /= projected_points[j].w;
					projected_points[j].z /= projected_points[j].w;
				}

				// Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space y values grow top->down
				projected_points[j].y *= -1;

				// Scale into the view			
				projected_points[j].x *= (display.window_width / 2.0f);
				projected_points[j].y *= (display.window_height / 2.0f);

				// Translate the projected points to the middle of the screen
				projected_points[j].x += (display.window_width / 2.0f);
				projected_points[j].y += (display.window_height / 2.0f);
			}


			// Calculate the shade intensity based on how aliged is the face normal and the opposite of the light direction
			float light_intensity_factor = -vec3_t::vec3_dot(normal, main_light.direction);

			// Calculate the triangle color based on the light angle
			uint32_t triangle_color = light_t::light_apply_intensity(mesh_face.color, light_intensity_factor);


			triangle_t triangle_to_render = {
				{
					{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
					{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
					{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
				},
				{
					{triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v},
					{triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v},
					{triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v}
				},
				triangle_color,
				texture_to_load
			};

			// Save the projected triangle in the array of triangles to render
			if (num_triangles_to_render < MAX_TRIANGLES) {
				triangles_to_render[num_triangles_to_render++] = triangle_to_render;
			}
		}
	}
}


void update(void) {
	// Wait some time until the reach the target frame time in milliseconds
	int wait_time = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	if (wait_time > 0 && wait_time <= FRAME_TARGET_TIME) {
		SDL_Delay(wait_time);
	}

	// Get a delta time factor converted to seconds to be used to update our game objects
	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

	previous_frame_time = SDL_GetTicks();

	// Initialize the counter of triangles to render for the current frame
	num_triangles_to_render = 0;

	for (int obj_index = 0; obj_index < object_mgr.meshes.size(); obj_index++) {
		mesh_t& mesh_to_render = object_mgr.meshes[obj_index];
		texture_t* texture_to_load = &object_mgr.textures[obj_index];


		// Change the mesh scale, rotation, and translation values per animation frame
		//mesh_to_render.rotation.x += 0.00f * delta_time;
		//mesh_to_render.rotation.y += 0.00f * delta_time;
		//mesh_to_render.rotation.z += 0.00f * delta_time;

		process_graphics_pipeline_stages(mesh_to_render, texture_to_load);

	}
}

void render() {

	display.draw_grid(0xFF444444);

	// Loop all projected triangles and render them
	for (int i = 0; i < num_triangles_to_render; i++) {
		triangle_t triangle = triangles_to_render[i];
		
		// Draw filled triangle
		if (display.render_method == Render_Method::RENDER_FILL_TRIANGLE || display.render_method == Render_Method::RENDER_FILL_TRIANGLE_WIRE) {
			triangle_t::draw_filled_triangle(display,
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // vertex A
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // vertex B
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // vertex C
				triangle.color
			);
		}

		// Draw textured triangle
		if (display.render_method == Render_Method::RENDER_TEXTURED || display.render_method == Render_Method::RENDER_TEXTURED_WIRE) {
			triangle_t::draw_textured_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, // vertex A
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v, // vertex B
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v, // vertex C
				display, triangle.texture
			);
		}

		// Draw triangle wireframe
		if (display.render_method == Render_Method::RENDER_WIRE || display.render_method == Render_Method::RENDER_WIRE_VERTEX || display.render_method == Render_Method::RENDER_FILL_TRIANGLE || display.render_method == Render_Method::RENDER_TEXTURED_WIRE) {
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

	display.render();
	display.clear_color_buffer(0xFF000000);
	display.clear_z_buffer();
}




int main(int argc, char* args[]) {
	is_running = display.initialize_window(SIMULATE_LOW_RESOLUTION, SIMULATED_WINDOW_HEIGHT, SIMULATED_WINDOW_WIDTH);

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	display.destroy_window();

	return 0;
}