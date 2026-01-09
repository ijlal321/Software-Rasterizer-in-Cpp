#pragma once
#include <vector>
#include "mesh.h"

class Object_Manager
{
	public:
		
		std::vector<mesh_t> meshes;
		std::vector<texture_t>  textures;
		
		static const int MAX_NR_OBJECTS = 10;

		Object_Manager() {};

		~Object_Manager() {};

		void load_object(const char* obj_filename, const char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation);

};

