#pragma once

#include "torrey.h"
#include "vector.h"
#include <variant>

struct PointLight {
    Vector3 intensity;
    Vector3 position;    
};

struct AreaLight {
    int shape_id;
    Vector3 radiance;
};

using Light = std::variant<PointLight, AreaLight>;
