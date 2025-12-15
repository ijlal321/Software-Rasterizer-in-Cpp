#pragma once
#include <vector>
#include "triangle.h"
#include "vector.h"

class mesh_t
{
public:
    std::vector<vec3_t> vertices;
	std::vector<face_t> faces;
    vec3_t rotation{0,0,0};

    void load_cube_mesh_data(void);
};

