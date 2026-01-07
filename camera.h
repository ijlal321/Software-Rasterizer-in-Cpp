#pragma once
#include "vector.h"

class camera_t
{
public:
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw;

    camera_t(vec3_t position, vec3_t direction, vec3_t forward_velocity, float yaw) : 
        position(position),
        direction(direction),
        forward_velocity(forward_velocity),
        yaw(yaw) 
    {};
};

