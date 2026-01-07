#include "mesh.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

static const int N_CUBE_VERTICES = 8;
static const int N_CUBE_FACES = (6 * 2); // 6 cube faces, 2 triangles per face

static vec3_t cube_mesh_vertices[N_CUBE_VERTICES] = {
    { -1,  -1,  -1}, // 1
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
    { 1,  2,  3, { 0, 1 }, { 0, 0 }, { 1, 0 }, 0xFFFFFFFF},
    { 1,  3,  4, { 0, 1 }, { 1, 0 }, { 1, 1 }, 0xFFFFFFFF},
    // right
    { 4,  3,  5, { 0, 1 }, { 0, 0 }, { 1, 0 }, 0xFFFFFFFF},
    { 4,  5,  6, { 0, 1 }, { 1, 0 }, { 1, 1 }, 0xFFFFFFFF},
    // back
    { 6,  5,  7, { 0, 1 }, { 0, 0 }, { 1, 0 }, 0xFFFFFFFF},
    { 6,  7,  8, { 0, 1 }, { 1, 0 }, { 1, 1 }, 0xFFFFFFFF},
    // left
    { 8,  7,  2, { 0, 1 }, { 0, 0 }, { 1, 0 }, 0xFFFFFFFF},
    { 8,  2,  1, { 0, 1 }, { 1, 0 }, { 1, 1 }, 0xFFFFFFFF},
    // top
    { 2,  7,  5, { 0, 1 }, { 0, 0 }, { 1, 0 }, 0xFFFFFFFF},
    { 2,  5,  3, { 0, 1 }, { 1, 0 }, { 1, 1 }, 0xFFFFFFFF},
    // bottom
    { 6,  8,  1, { 0, 1 }, { 0, 0 }, { 1, 0 }, 0xFFFFFFFF},
    { 6,  1,  4, { 0, 1 }, { 1, 0 }, { 1, 1 }, 0xFFFFFFFF}
};


void mesh_t::load_cube_mesh_data(void) {
	vertices.clear();
	faces.clear();
    

	vertices.assign(cube_mesh_vertices, cube_mesh_vertices + N_CUBE_VERTICES);
	faces.assign(cube_mesh_faces, cube_mesh_faces + N_CUBE_FACES);
}

void mesh_t::load_obj_file_data(const char * path) {
    FILE* file;
    file = fopen(path, "r");
    char line[1024];

    if (!file) {
        std::cerr << "Could not open the file: " << path << std::endl;
        return;
	}

	vertices.clear();
	faces.clear();

    std::vector<tex2_t> texcoords;

    while (fgets(line, 1024, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        }
        // Texture coordinate information
        if (strncmp(line, "vt ", 3) == 0) {
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            texcoords.push_back(texcoord);
        }
        // Face information
        if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]
            );
            face_t face = {
                vertex_indices[0],
                vertex_indices[1],
                vertex_indices[2],
                texcoords[texture_indices[0] - 1],
                texcoords[texture_indices[1] - 1],
                texcoords[texture_indices[2] - 1],
                0xFFFFFFFF
            };
            faces.push_back(face);
        }
    }
    fclose(file);
}

