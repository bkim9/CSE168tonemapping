#pragma once

#include "torrey.h"
#include "vector.h"

struct Intersection {
    Vector3 position;
    Vector3 normal;
    Real distance;
    int material_id;
    int area_light_id;
    double u;
    double v;
};