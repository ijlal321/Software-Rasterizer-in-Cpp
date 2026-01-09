#include "object_manager.h"


void Object_Manager::load_object(const char* obj_filename, const char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation) {
	if (meshes.size() >= MAX_NR_OBJECTS) {
		return;
	}

	meshes.emplace_back(mesh_t());
	textures.emplace_back(texture_t());

	mesh_t& mesh = meshes.back();
	texture_t& texture = textures.back();

	mesh.load_obj_file_data(obj_filename);
	texture.load_png_texture_data(png_filename);

	mesh.rotation = rotation;
	mesh.scale = scale;
	mesh.translation = translation;

}
