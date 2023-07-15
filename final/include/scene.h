#pragma once

#include "image.h"
#include "torrey.h"
#include "bvh.h"
#include "camera.h"
#include "light.h"
#include "intersection.h"
#include "material.h"
#include "parse_scene.h"
#include "shape.h"
#include <optional>
#include "flexception.h"
#include "ray.h"
#include <algorithm>

struct Scene {
    Scene() {
        // default cam
        Vector3 lookfrom(0,0,-1);
        Vector3 lookat(0,0,0);
        Vector3 up(0,1,0);
        Real vfov = Real(120);
        Camera cam{lookfrom,lookat,up,vfov};
        Camera camera = cam;
        // default width and height 
        width = 1920;
        height = 1080;
        shapes.clear();
        materials.clear();
        lights.clear();
        background_color = Vector3(.1,.2,.3);
        samples_per_pixel = 3;
        meshes.clear();
        bvh_nodes.clear();
        bvh_root_id= -1;
    }
    Scene(const ParsedScene &scene);

    Camera camera;
    int width, height;
    std::vector<Shape> shapes;
    std::vector<Material> materials;
    std::vector<Light> lights;
    Vector3 background_color;

    int samples_per_pixel;
    // For the Triangle in the shapes to reference to.
    std::vector<TriangleMesh> meshes;

    std::vector<BVHNode> bvh_nodes;
    int bvh_root_id;
};

std::optional<Intersection> intersect(const Scene &scene, Ray ray);
bool occluded(const Scene &scene, const Ray &ray);
Vector3 evalDiffuse(const Scene &scene, MaterialBase mat, pcg32_state rng);