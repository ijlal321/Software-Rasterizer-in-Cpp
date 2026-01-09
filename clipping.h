#pragma once


#include "triangle.h"
#include "vector.h"

#define MAX_NUM_POLY_VERTICES 10
#define MAX_NUM_POLY_TRIANGLES 10
#define NUM_PLANES 6

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

class plane_t {
public:
    vec3_t point;
    vec3_t normal;

	plane_t() {}
	plane_t(const vec3_t p, const vec3_t n) : point(p), normal(n) {}

};


class polygon_t {
public:
    vec3_t vertices[MAX_NUM_POLY_VERTICES];
    tex2_t texcoords[MAX_NUM_POLY_VERTICES];
    int num_vertices;
};

class frustrum_t {
public:
    plane_t frustum_planes[NUM_PLANES];

    void init_frustum_planes(float fov_x, float fov_y, float z_near, float z_far);
    void clip_polygon(polygon_t* polygon);
    
    static polygon_t polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0, tex2_t t1, tex2_t t2);
    static void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles);

private:
    void clip_polygon_against_plane(polygon_t* polygon, int plane);
};


