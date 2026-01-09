#pragma once
#include "vector.h"

class camera_t
{
public:
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw;
    float pitch;

    camera_t(vec3_t position, vec3_t direction, vec3_t forward_velocity, float yaw) : 
        position(position),
        direction(direction),
        forward_velocity(forward_velocity),
        yaw(yaw) 
    {};



    vec3_t get_camera_lookat_target();
};

