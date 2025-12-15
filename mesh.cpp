#include "mesh.h"

static const int N_CUBE_VERTICES = 8;
static const int N_CUBE_FACES = (6 * 2); // 6 cube faces, 2 triangles per face

static vec3_t cube_mesh_vertices[N_CUBE_VERTICES] = {
    { -1,  -1,  -1 }, // 1
    { -1,  1,  -1 }, // 2
    { 1,  1,  -1 }, // 3
    { 1,  -1,  -1 }, // 4
    { 1,  1,  1 }, // 5
    { 1,  -1,  1 }, // 6
    { -1,  1,  1 }, // 7
    { -1,  -1,  1 }  // 8
};

static face_t cube_mesh_faces[N_CUBE_FACES] = {
    // front
    { 1,  2,  3 },
    { 1,  3,  4 },
    // right
    { 4,  3,  5 },
    { 4,  5,  6 },
    // back
    { 6,  5,  7 },
    { 6,  7,  8 },
    // left
    { 8,  7,  2 },
    { 8,  2,  1 },
    // top
    { 2,  7,  5 },
    { 2,  5,  3 },
    // bottom
    { 6,  8,  1 },
    { 6,  1,  4 }
};


void mesh_t::load_cube_mesh_data(void) {
	vertices.clear();
	faces.clear();
    

	vertices.assign(cube_mesh_vertices, cube_mesh_vertices + N_CUBE_VERTICES);
	faces.assign(cube_mesh_faces, cube_mesh_faces + N_CUBE_FACES);
}