#include <iostream>
#include <vector>
#include <SDL.h>
#include <algorithm>
#include "display.h"
#include "vector.h"
#include "Mesh.h"
# include "matrix.h"
#include "light.h"
#include "texture.h"

std::vector<triangle_t> triangles_to_render; // triangles given to 
mesh_t cube_mesh;  // Main Object we Display

bool is_running = false;
std::vector<uint32_t> color_buffer;
Display display (color_buffer);

tex2_t texture_to_load;

vec3_t camera_position = { 0, 0, 0 };
mat4_t proj_matrix;
light_t main_light = { {0, 0, 1} };

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
		if (event.key.keysym.sym == SDLK_5)
			display.render_method = Render_Method::RENDER_TEXTURED;
		if (event.key.keysym.sym == SDLK_6)
			display.render_method = Render_Method::RENDER_TEXTURED_WIRE;
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
	display.render_method = Render_Method::RENDER_TEXTURED_WIRE;
	display.cull_method = Cull_Method::CULL_BACKFACE;

	display.setup();

	// Initialize the perspective projection matrix
	float fov = M_PI / 3.0; // the same as 180/3, or 60deg
	float aspect = (float)display.window_height / (float)display.window_width;
	float znear = 0.1;
	float zfar = 100.0;
	proj_matrix = mat4_t::mat4_make_perspective(fov, aspect, znear, zfar);

	texture_to_load.load_redbrick_texture();

	cube_mesh.load_cube_mesh_data();
	//cube_mesh.load_obj_file_data("assets/cube.obj");
	//cube_mesh.load_obj_file_data("assets/f22.obj");

}

void update(void) {
	// Wait some time until the reach the target frame time in milliseconds
	int wait_time = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	if (wait_time > 0 && wait_time <= FRAME_TARGET_TIME) {
		SDL_Delay(wait_time);
	}
	previous_frame_time = SDL_GetTicks();

	// Change the mesh scale, rotation, and translation values per animation frame
	//cube_mesh.rotation.x += 0.01f;
	//cube_mesh.rotation.y += 0.01f;
	//cube_mesh.rotation.z += 0.01;
	cube_mesh.translation.z = 5.0f;

	// Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_t::mat4_make_scale(cube_mesh.scale.x, cube_mesh.scale.y, cube_mesh.scale.z);
	mat4_t translation_matrix = mat4_t::mat4_make_translation(cube_mesh.translation.x, cube_mesh.translation.y, cube_mesh.translation.z);
	mat4_t rotation_matrix_x = mat4_t::mat4_make_rotation_x(cube_mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_t::mat4_make_rotation_y(cube_mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_t::mat4_make_rotation_z(cube_mesh.rotation.z);


	for (int i = 0; i < cube_mesh.faces.size(); i++) {
	//for (int i = 0; i < 1; i++) {
		face_t mesh_face = cube_mesh.faces[i]; // get face [3d] from mesh
		
		vec3_t face_vertices[3]; // store vertexes of face
		face_vertices[0] = cube_mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = cube_mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = cube_mesh.vertices[mesh_face.c - 1];
	
		vec4_t transformed_vertices[3];

		// Loop all three vertices of this current face and apply transformations

		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

			// Create a World Matrix combining scale, rotation, and translation matrices
			mat4_t world_matrix = mat4_t::mat4_identity();

			// Order matters: First scale, then rotate, then translate. [T]*[R]*[S]*v
			world_matrix = mat4_t::mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_t::mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_t::mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_t::mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_t::mat4_mul_mat4(translation_matrix, world_matrix);

			// Multiply the world matrix by the original vector
			transformed_vertex = mat4_t::mat4_mul_vec4(world_matrix, transformed_vertex);

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
		vec3_t camera_ray = vec3_t::vec3_sub(camera_position, vector_a);

		// Backface culling test to see if the current face should be projected
		if (display.cull_method == Cull_Method::CULL_BACKFACE) {

			// Calculate how aligned the camera ray is with the face normal (using dot product)
			float dot_normal_camera = vec3_t::vec3_dot(normal, camera_ray);

			// Bypass the triangles that are looking away from the camera
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		vec4_t projected_points[3];

		// Loop all three vertices to perform projection
		for (int j = 0; j < 3; j++) {
			// Project the current vertex
			//projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));
			projected_points[j] = mat4_t::mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

			// Scale into the view
			projected_points[j].x *= (display.window_width / 2.0f);
			projected_points[j].y *= (display.window_height / 2.0f);

			projected_points[j].y *= -1; // Invert Y axis

			// Translate the projected points to the middle of the screen
			projected_points[j].x += (display.window_width / 2);
			projected_points[j].y += (display.window_height / 2);
		}


		// Calculate the average depth for each face based on the vertices after transformation
		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;


		// Calculate the shade intensity based on how aliged is the face normal and the opposite of the light direction
		float light_intensity_factor = -vec3_t::vec3_dot(normal, main_light.direction);

		// Calculate the triangle color based on the light angle
		uint32_t triangle_color = light_t::light_apply_intensity(mesh_face.color, light_intensity_factor);


		triangle_t projected_triangle = {
			{
				{ projected_points[0].x, projected_points[0].y },
				{ projected_points[1].x, projected_points[1].y },
				{ projected_points[2].x, projected_points[2].y },
			},
			{
				mesh_face.a_uv,
				mesh_face.b_uv,
				mesh_face.c_uv
			},
			triangle_color,
			avg_depth
		};

		// Save the projected triangle in the array of triangles to render
		triangles_to_render.push_back(projected_triangle);

	}
}

void render() {

	display.draw_grid(0xFF444444);

	// sort triangles to render by average depth (painter's algorithm)
	std::sort(triangles_to_render.begin(), triangles_to_render.end(), [](triangle_t& t1, triangle_t& t2) {
		return t1.avg_depth > t2.avg_depth;
		}
	);

	// Loop all projected triangles and render them
	for (int i = 0; i < triangles_to_render.size(); i++) {
		triangle_t triangle = triangles_to_render[i];
		
		// Draw filled triangle
		if (display.render_method == Render_Method::RENDER_FILL_TRIANGLE || display.render_method == Render_Method::RENDER_FILL_TRIANGLE_WIRE) {
			triangle_t::draw_filled_triangle(display,
				triangle.points[0].x, triangle.points[0].y,
				triangle.points[1].x, triangle.points[1].y,
				triangle.points[2].x, triangle.points[2].y,
				triangle.color
			);
		}

		// Draw textured triangle
		if (display.render_method == Render_Method::RENDER_TEXTURED || display.render_method == Render_Method::RENDER_TEXTURED_WIRE) {
			display.draw_textured_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.texcoords[0].u, triangle.texcoords[0].v, // vertex A
				triangle.points[1].x, triangle.points[1].y, triangle.texcoords[1].u, triangle.texcoords[1].v, // vertex B
				triangle.points[2].x, triangle.points[2].y, triangle.texcoords[2].u, triangle.texcoords[2].v, // vertex C
				texture_to_load
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