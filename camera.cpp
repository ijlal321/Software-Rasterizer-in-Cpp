#include "camera.h"
#include "matrix.h"
#include "vector.h"

vec3_t camera_t::get_camera_lookat_target(void) {
    // Initialize the target looking at the positive z-axis
    vec3_t target = { 0, 0, 1 };

    mat4_t camera_yaw_rotation = mat4_t::mat4_make_rotation_y(yaw);
    mat4_t camera_pitch_rotation = mat4_t::mat4_make_rotation_x(pitch);

    // Create camera rotation matrix based on yaw and pitch
    mat4_t camera_rotation = mat4_t::mat4_identity();
    camera_rotation = mat4_t::mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
    camera_rotation = mat4_t::mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

    // Update camera direction based on the rotation
    vec4_t camera_direction = mat4_t::mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
    direction = vec3_from_vec4(camera_direction);

    // Offset the camera position in the direction where the camera is pointing at
    target = vec3_t::vec3_add(position, direction);

    return target;
}
