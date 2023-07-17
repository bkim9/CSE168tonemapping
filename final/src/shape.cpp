#include "shape.h"


/// Numerically stable quadratic equation solver at^2 + bt + c = 0
/// See https://people.csail.mit.edu/bkph/articles/Quadratics.pdf
/// returns false when it can't find solutions.
inline bool solve_quadratic(Real a, Real b, Real c, Real *t0, Real *t1) {
    // Degenerated case
    if (a == 0) {
        if (b == 0) {
            return false;
        }
        *t0 = *t1 = -c / b;
        return true;
    }

    Real discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return false;
    }
    Real root_discriminant = sqrt(discriminant);
    if (b >= 0) {
        *t0 = (- b - root_discriminant) / (2 * a);
        *t1 = 2 * c / (- b - root_discriminant);
    } else {
        *t0 = 2 * c / (- b + root_discriminant);
        *t1 = (- b + root_discriminant) / (2 * a);
    }
    return true;
}

inline static void get_sphere_uv(const Vector3& p, double &u, double &v) {
    auto theta = acos(-p.y);
    auto phi = atan2(-p.z,p.x) + c_PI;

    u = phi   / (2 * c_PI);
    v = theta / c_PI;
}

inline std::optional<Intersection> intersect(const Sphere &sph, const Ray &ray) {
    // Our sphere is ||p - x||^2 = r^2
    // substitute x = o + d * t, we want to solve for t
    // ||p - (o + d * t)||^2 = r^2
    // (p.x - (o.x + d.x * t))^2 + (p.y - (o.y + d.y * t))^2 + (p.z - (o.z + d.z * t))^2 - r^2 = 0
    // (d.x^2 + d.y^2 + d.z^2) t^2 + 2 * (d.x * (o.x - p.x) + d.y * (o.y - p.y) + d.z * (o.z - p.z)) t + 
    // ((p.x-o.x)^2 + (p.y-o.y)^2 + (p.z-o.z)^2  - r^2) = 0
    // A t^2 + B t + C
    Vector3 v = ray.org - sph.center;
    Real A = dot(ray.dir, ray.dir);
    Real B = 2 * dot(ray.dir, v);
    Real C = dot(v, v) - sph.radius * sph.radius;
    Real t0, t1;
    if (!solve_quadratic(A, B, C, &t0, &t1)) {
        // No intersection
        return {};
    }
    if (t0 > t1) {
        std::swap(t0, t1);
    }
    Real t = t0;
    if (t0 >= ray.tnear && t0 < ray.tfar) {
        t = t0;
    }
    if (t1 >= ray.tnear && t1 < ray.tfar && t < ray.tnear) {
        t = t1;
    }

    if (t >= ray.tnear && t < ray.tfar) {
        // Record the intersection
        Vector3 p = ray.org + t * ray.dir;
        Vector3 n = normalize(p - sph.center);
        double textU, textV;
        get_sphere_uv((p-sph.center)/sph.radius, textU, textV);
        return Intersection{p, n, t, sph.material_id, sph.area_light_id, textU,textV};
    }
    return {};
}

std::optional<Vector3>
    intersect_triangle(const Ray &ray,
                       const Vector3 &p0,
                       const Vector3 &p1,
                       const Vector3 &p2) {
    Vector3 e1 = p1 - p0;
    Vector3 e2 = p2 - p0;
    Vector3 s1 = cross(ray.dir, e2);
    Real divisor = dot(s1, e1);
    if (divisor == 0) {
        return {};
    }
    Real inv_divisor = 1 / divisor;
    Vector3 s = ray.org - p0;
    Real u = dot(s, s1) * inv_divisor;
    Vector3 s2 = cross(s, e1);
    Real v = dot(ray.dir, s2) * inv_divisor;
    Real t = dot(e2, s2) * inv_divisor;

    if (t > ray.tnear && t < ray.tfar && u >= 0 && v >= 0 && u + v <= 1) {
        return Vector3{u, v, t};
    }

    return {};
}

inline std::optional<Intersection> intersect(const Triangle &tri, const Ray &ray) {
	const TriangleMesh &mesh = *tri.mesh;
	Vector3i index = mesh.indices[tri.face_index];
	Vector3 p0 = mesh.positions[index[0]];
	Vector3 p1 = mesh.positions[index[1]];
	Vector3 p2 = mesh.positions[index[2]];
    Vector3 n0 = mesh.normals.size()? mesh.normals[index[0]] : Vector3(0,0,0);
    Vector3 n1 = mesh.normals.size()? mesh.normals[index[1]] : Vector3(0,0,0);
    Vector3 n2 = mesh.normals.size()? mesh.normals[index[2]] : Vector3(0,0,0);
    auto uv0 = mesh.uvs.size()? mesh.uvs[index[0]] : Vector2(1.0,1.0);
    auto uv1 = mesh.uvs.size()? mesh.uvs[index[1]] : Vector2(1.0,1.0);
    auto uv2 = mesh.uvs.size()? mesh.uvs[index[2]] : Vector2(1.0,1.0);
	if (auto uvt_ = intersect_triangle(ray, p0, p1, p2)) {
		Vector2 uv = Vector2{uvt_->x, uvt_->y};
		Real t = uvt_->z;
		Vector3 p = (1 - uv[0] - uv[1]) * p0 + uv[0] * p1 + uv[1] * p2;
		Vector3 n = mesh.normals.size()? (1 - uv[0] - uv[1]) * n0 + uv[0] * n1 + uv[1] * n2: (cross(p1 - p0, p2 - p0));
        auto textureU = (1-uvt_->x-uvt_->y)* uv0.x + uvt_->x * uv1.x + uvt_->y * uv2.x;
        auto textureV = (1-uvt_->x-uvt_->y)* uv0.y + uvt_->x * uv1.y + uvt_->y * uv2.y;
		return Intersection{p, n, t, mesh.material_id, mesh.area_light_id, textureU, textureV};
	}
	return {};
}

std::optional<Intersection> intersect(const Shape &shape, const Ray &ray) {
    std::optional<Intersection> temp_rec;
	if (auto *sph = std::get_if<Sphere>(&shape)) {
		temp_rec = intersect(*sph, ray);
	} else if(auto *tri = std::get_if<Triangle>(&shape)) {
		temp_rec = intersect(*tri, ray);
	} else {
		assert(false);
		return {};
	}
    if (dot(-ray.dir, temp_rec->normal) < 0) {
        temp_rec->normal = -temp_rec->normal;
    }
    return temp_rec;
}

bool occluded(const Shape &shape, const Ray &ray) {
    return bool(intersect(shape, ray));
}

// o - v > /_\.
Real Triangle::pdf_value( const Vector3& o, const Vector3& v) const {
    auto hit = intersect(*this, Ray{o, v, 0.00001, infinity<Real>()});
    if (!hit) return 0;
    // ----------------------------old version----------------------------
    // Real distance_squared = hit->distance * hit->distance;
    // Real cosine = fabs(dot(v, hit->normal)) / length(v);
    // Real area = length(crossproduct)/2.;
    // return distance_squared / ( cosine * area );
    // ----------------------------old version----------------------------
    auto A = p0 - o;
    auto B = p1 - o;
    auto C = p2 - o;
    auto st = spherical_triangle(A,B,C);
    return 1/st.area;
}

    // 
    //  shape |
    //   -(x)-
    //     \.
    //      wo   /
    //        o==p
    // x should be on the triangle
Vector3 Triangle::random(const Vector3& o, Vector3& n_x, Real& Area, pcg32_state rng) const {
    n_x = normalize(crossproduct);
    auto x = random_from_sphere(o, rng);
    if( auto intsec = intersect(*this, Ray{o, x-o, Real(.001),infinity<Real>()})) {
        x = intsec->position;
    } else {
        x= Vector3(0,0,0);
    }
    if ( dot(n_x, o - x) < 0) n_x = -n_x;
    return x;
}
