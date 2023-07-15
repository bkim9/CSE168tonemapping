#pragma once

#include "torrey.h"
#include "vector.h"
#include "intersection.h"
#include <optional>
#include <variant>
#include <vector>
#include "ray.h"
#include "pcg.h"
#include "onb.h"

struct Ray;

inline Vector3 hemiloc(Real r1, Real z) {
    auto phi = 2. * c_PI * r1;
    auto sine_theta = sqrt(1.-z*z);
    auto x = cos(phi) * sine_theta;
    auto y = sin(phi) * sine_theta;
    return Vector3(x,y,z);
}

struct ShapeBase {
    int material_id = -1;
    int area_light_id = -1;
};

struct Sphere : public ShapeBase {
    Vector3 center;
    Real radius;

    // cone sampling pdf
    Real pdf_value( const Vector3& o, const Vector3& v) const {
        auto r = radius;
        // if (!intersect(*this, Ray{o, v, 0.00001, infinity<Real>()})) return 0;
        auto sine_theta_max_squaured = r * r / length_squared(center - o); 
        auto cos_theta_max = sqrt(1 - sine_theta_max_squaured);
        auto solid_angle = 2 * c_PI * (1-cos_theta_max);
        return 1 / solid_angle;
    }
    // cone sampling assuming center is the origin
    Vector3 random_to_sphere( Real distance_squared, Real& area, pcg32_state rng) const {
        auto r = radius;
        pcg32_state rng_ = init_pcg32(time(NULL));
        auto r1 = next_pcg32_real<Real>(rng_);
        auto r2 = next_pcg32_real<Real>(rng_);
        // auto r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        // auto r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        auto sine_theta_max_squaured = r * r / distance_squared; 
        auto cos_thata_max = sqrt(1 - sine_theta_max_squaured);
        auto z = 1 - r2 * (1 - cos_thata_max);
        area = 2 * c_PI * (1 - cos_thata_max);
        return r * hemiloc(r1,z);
    }
    // center -direction>  o
    Vector3 random(const Vector3& o, Vector3& n_x, Real& area, pcg32_state rng) const {
        Vector3 direction = o - center;
        onb uvw;
        auto distance_squared = length_squared(direction);
        uvw.build_from_w(direction);
        n_x = uvw.local(random_to_sphere(distance_squared, area, rng));
        auto x = center + n_x;
        n_x = normalize(n_x);
        return x;
    }
};

struct TriangleMesh : public ShapeBase {
    std::vector<Vector3> positions;
    std::vector<Vector3i> indices;
    std::vector<Vector3> normals;
    std::vector<Vector2> uvs;
};

struct Triangle {
    int face_index;
    const TriangleMesh *mesh;
    Vector3 p0, p1, p2, n0, n1, n2, crossproduct;

    void setup() {
        auto ps = mesh->positions;
        auto ns = mesh->normals;
        auto is = mesh->indices[face_index];  
        p0 = ps[is[0]];
        p1 = ps[is[1]];
        p2 = ps[is[2]];
        n0 = ns[is[0]];
        n1 = ns[is[1]];
        n2 = ns[is[2]];
        crossproduct = cross(p1-p0,p2-p0);
    }
    Real pdf_value( const Vector3& o, const Vector3& v) const;


    Vector3 random(const Vector3& o, Vector3& n_x, Real& Area, pcg32_state rng) const {
        auto u1 = next_pcg32_real<Real>(rng);
        auto u2 = next_pcg32_real<Real>(rng);
        auto b1 = 1 - sqrt(u1);
        auto b2 = u2 * sqrt(u1);
        Area = length(crossproduct) / 2.0;
        n_x = normalize(crossproduct);
        auto x = (1 - b1 - b2) * p0 + b1 * p1 + b2 * p2;
        auto shading_n = (1 - b1 - b2) * n0 + b1 * n1 + b2 * n2;
        if ( dot(n_x, shading_n) < 0) n_x = -n_x;
        return x;
    }
};

using Shape = std::variant<Sphere, TriangleMesh, Triangle>;

std::optional<Intersection> intersect(const Shape &shape, const Ray &ray);
bool occluded(const Shape &shape, const Ray &ray);

std::optional<Vector3>
    intersect_triangle(const Ray &ray,
                       const Vector3 &p0,
                       const Vector3 &p1,
                       const Vector3 &p2);


