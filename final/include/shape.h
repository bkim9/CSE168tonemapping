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
#include <ctime>

struct Ray;

struct SphericalTriangle {
    Vector3 cosines;
    Real cos_c;
    Real area;
};

inline Vector3 hemiloc(Real r1, Real z) {
    auto phi = 2. * c_PI * r1;
    auto sine_theta = sqrt(1.-z*z);
    auto x = cos(phi) * sine_theta;
    auto y = sin(phi) * sine_theta;
    return Vector3(x,y,z);
}

// y is assumed to be a unitvector
inline Vector3 orthonize ( Vector3 x, Vector3 y) {
    return normalize(x - dot(x,y) * y);
}

inline SphericalTriangle spherical_triangle(Vector3 A, Vector3 B,Vector3 C) {
    Real cos_alpha = dot(cross(B,A), cross(C,A));
    Real sin_alpha = sqrt(1-cos_alpha*cos_alpha);
    Real cos_beta =  dot(cross(A,B), cross(C,B));
    Real cos_gamma = dot(cross(A,C), cross(B,C));
    Real cos_c = dot(A,B);
    Real area_sphrere = acos(cos_alpha) + acos(cos_beta) + acos(cos_gamma) - c_PI;
    return SphericalTriangle{Vector3(cos_alpha,cos_beta,cos_gamma), cos_c, area_sphrere };
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
        auto sine_theta_max_squaured = r * r / length_squared(center - o); 
        auto cos_theta_max = sqrt(1 - sine_theta_max_squaured);
        auto solid_angle = 2 * c_PI * (1-cos_theta_max);
        return 1 / solid_angle;
    }
    // cone sampling assuming center is the origin
    Vector3 random_to_sphere( Real distance_squared, Real& area, pcg32_state rng) const {
        auto r = radius;
        auto r1 = next_pcg32_real<Real>(rng);
        auto r2 = next_pcg32_real<Real>(rng);
        auto sin_theta_max = sqrt( r * r / distance_squared );  
        auto cone_depth = 1-sin_theta_max; // depth of unit icecream on a cone
        auto z = 1 - r2 * cone_depth;
        area = 2 * c_PI * cone_depth *r*r;
        return r * hemiloc(r1,z);
    }
    // x: on the sphere
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
    
    // input:  center of sphere(c)
    // output: sample point on spherical triangle in radius 1 and center being the input
    Vector3 random_from_sphere(const Vector3& c, pcg32_state rng ) const {
        Vector3 A = normalize(p0 - c);
        Vector3 B = normalize(p1 - c);
        Vector3 C = normalize(p2 - c);

        auto st = spherical_triangle(A,B,C);

        Real cos_alpha = st.cosines.x;
        Real sin_alpha = sqrt(1-cos_alpha*cos_alpha);
        Real cos_beta =  st.cosines.y;
        Real cos_gamma = st.cosines.z;
        Real cos_c = st.cos_c;
        auto r1 = next_pcg32_real<Real>(rng);
        auto r2 = next_pcg32_real<Real>(rng);
        
        Real area_hat = r1*st.area;
        Real cosAhat = cos(r1*area_hat);
        Real sinAhat = sin(r1*area_hat);
        auto s = sin(area_hat - acos(cos_alpha));
        auto t = cos(area_hat - acos(cos_alpha));
        auto u = t - cos_alpha;
        auto v = s + sin_alpha * cos_c;
        auto q = ((v*t - u*s) * cos_alpha - v) / (v*s + u*t) / sin_alpha;
        auto C_hat = q * A + sqrt(1-q*q) * orthonize(C,A);
        auto z = 1 - r2 * (1- dot(C_hat,B));

        return z*B + sqrt(1-z*z) * orthonize(C_hat,B) + c;
    }

    // x should be on the triangle
    Vector3 random(const Vector3& o, Vector3& n_x, Real& Area, pcg32_state rng) const {
        n_x = normalize(crossproduct);
        auto x = random_from_sphere(o, rng);
        // ----------------------------old version----------------------------
        // auto u1 = next_pcg32_real<Real>(rng);
        // auto u2 = next_pcg32_real<Real>(rng);
        // auto b1 = 1 - sqrt(u1);
        // auto b2 = u2 * sqrt(u1);
        // Area = length(crossproduct) / 2.0;
        // auto x = (1 - b1 - b2) * p0 + b1 * p1 + b2 * p2;
        // ----------------------------old version----------------------------
        auto shading_n = x - o;
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



