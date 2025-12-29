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

void mesh_t::load_obj_file_data(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open OBJ file: " + path);
	}

	vertices.clear();
	faces.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream line_stream(line);
        std::string word;
        
        if (!(line_stream >> word)) {
			continue; // skip empty lines
        }

        if (word == "v") {
            vec3_t vertex;
            line_stream >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (word == "f") {
            face_t face;
            char slash; // to handle possible texture/normal indices
            int dummy_num;
            line_stream >> face.a >> slash >> dummy_num >> slash >> dummy_num
                        >> face.b >> slash >> dummy_num >> slash >> dummy_num
                        >> face.c;
            face.color = 0xFFFFFFFF;
            faces.push_back(face);
		}
    }

    file.close();
}

