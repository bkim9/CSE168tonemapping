#include "scene.h"


Scene::Scene(const ParsedScene &scene) :
        camera(from_parsed_camera(scene.camera)),
        width(scene.camera.width),
        height(scene.camera.height),
        background_color(scene.background_color),
        samples_per_pixel(scene.samples_per_pixel) {
    // Extract triangle meshes from the parsed scene.
    int tri_mesh_count = 0;
    for (const ParsedShape &parsed_shape : scene.shapes) {
        if (std::get_if<ParsedTriangleMesh>(&parsed_shape)) {
            tri_mesh_count++;
        }
    }
    meshes.resize(tri_mesh_count);

    // Extract the shapes
    tri_mesh_count = 0;
    for (int i = 0; i < (int)scene.shapes.size(); i++) {
        const ParsedShape &parsed_shape = scene.shapes[i];
        if (auto *sph = std::get_if<ParsedSphere>(&parsed_shape)) {
            shapes.push_back(
                Sphere{{sph->material_id, sph->area_light_id},
                    sph->position, sph->radius});
        } else if (auto *parsed_mesh = std::get_if<ParsedTriangleMesh>(&parsed_shape)) {
            meshes[tri_mesh_count] = TriangleMesh{
                {parsed_mesh->material_id, parsed_mesh->area_light_id},
                parsed_mesh->positions, parsed_mesh->indices,
                parsed_mesh->normals, parsed_mesh->uvs};
            // Extract all the individual triangles
            for (int face_index = 0; face_index < (int)parsed_mesh->indices.size(); face_index++) {
                auto triangle = Triangle{face_index, &meshes[tri_mesh_count]};
                triangle.setup();
                shapes.push_back(triangle);

                if( face_index % 10000 == 0 )
                    std::cout<< "   working on face_index: " << face_index  << "/" << (int)parsed_mesh->indices.size()<< std::endl;
            }
            std::cout<< "working on tri [ " << tri_mesh_count << " ]" << std::endl;
            tri_mesh_count++;
        } else {
            assert(false);
        }
        std::cout<< "working on shape [ " << i << " ]" << std::endl;
    }

    std::cout << "extracting shapes done "<< std::endl;
    // Copy the materials
    for (const ParsedMaterial &parsed_mat : scene.materials) {
        if (auto *diffuse = std::get_if<ParsedDiffuse>(&parsed_mat)) {
            Diffuse d;
            d.isdiffuse = true;
            auto parsed_texture = std::get_if<ParsedImageTexture>(&diffuse->reflectance);
            if ( parsed_texture ) {
                d.reflectance = Vector3(0,0,0);
                d.texture = new Texture(parsed_texture);
            } else {
                d.reflectance = std::get<Vector3>(diffuse->reflectance);
                d.texture = nullptr;
            }
            materials.push_back(d);
        } else if (auto *mirror = std::get_if<ParsedMirror>(&parsed_mat)) {     
            Mirror m;      
            m.ismirror = true;
            auto parsed_texture = std::get_if<ParsedImageTexture>(&mirror->reflectance);
            if ( parsed_texture ) {
                m.reflectance = Vector3(0,0,0);
                m.texture = new Texture(parsed_texture);
            } else {
                m.reflectance = std::get<Vector3>(mirror->reflectance);
                m.texture = nullptr;
            }
            materials.push_back(m);
        } else if (auto *plastic = std::get_if<ParsedPlastic>(&parsed_mat)) {
            Plastic p;
            p.isplastic = true;
            p.eta = plastic->eta;
            auto parsed_texture = std::get_if<ParsedImageTexture>(&plastic->reflectance);
            if ( parsed_texture ) {
                p.reflectance = Vector3(0,0,0);
                p.texture = new Texture(parsed_texture);
            } else {
                p.reflectance = std::get<Vector3>(plastic->reflectance);
                p.texture = nullptr;
            }
            materials.push_back(p);
        } else if ( auto *phong = std::get_if<ParsedPhong>(&parsed_mat)) {
            Phong ph;
            ph.isphong = true;
            ph.exponent = phong->exponent;
            auto parsed_texture = std::get_if<ParsedImageTexture>(&phong->reflectance);
            if ( parsed_texture ) {
                ph.reflectance = Vector3(0,0,0);
                ph.texture = new Texture(parsed_texture);
            } else {
                ph.reflectance = std::get<Vector3>(phong->reflectance);
                ph.texture = nullptr;
            }
            materials.push_back(ph);
        } else if (auto *blinn = std:: get_if<ParsedBlinnPhong>(&parsed_mat)) {
            BlinnPhong blinnph;
            blinnph.isblinn = true;
            blinnph.exponent = blinn->exponent;
            blinnph.reflectance = std::get<Vector3>(blinn->reflectance);
            auto parsed_texture = std::get_if<ParsedImageTexture>(&blinn->reflectance);
            blinnph.texture = parsed_texture? new Texture(parsed_texture): nullptr;
            materials.push_back(blinnph);
        } else if (auto *micro = std::get_if<ParsedBlinnPhongMicrofacet>(&parsed_mat)) {
            BlinnPhongMicrofacet microph;
            microph.ismicro = true;
            microph.exponent = micro->exponent;
            microph.reflectance = std::get<Vector3>(micro->reflectance);
            auto parsed_texture = std::get_if<ParsedImageTexture>(&micro->reflectance);
            microph.texture = parsed_texture? new Texture(parsed_texture): nullptr;
            materials.push_back(microph);
        }
    }

    // Copy the lights
    for (const ParsedLight &parsed_light : scene.lights) {
        // We assume the light is point light for now.
        if (auto *point_light = std::get_if<ParsedPointLight>(&parsed_light)) {
            lights.push_back(PointLight{point_light->intensity, point_light->position});
        } else if (auto *area_light = std::get_if<ParsedDiffuseAreaLight>(&parsed_light)) {
            lights.push_back(AreaLight{area_light->shape_id, area_light->radiance});
        }
    }

    // Build BVH
    std::vector<BBoxWithID> bboxes(shapes.size());
    for (int i = 0; i < (int)bboxes.size(); i++) {
        if (auto *sph = std::get_if<Sphere>(&shapes[i])) {
            Vector3 p_min = sph->center - sph->radius;
            Vector3 p_max = sph->center + sph->radius;
            bboxes[i] = {BBox{p_min, p_max}, i};
        } else if (auto *tri = std::get_if<Triangle>(&shapes[i])) {
            const TriangleMesh *mesh = tri->mesh;
            Vector3i index = mesh->indices[tri->face_index];
            Vector3 p0 = mesh->positions[index[0]];
            Vector3 p1 = mesh->positions[index[1]];
            Vector3 p2 = mesh->positions[index[2]];
            Vector3 p_min = min(min(p0, p1), p2);
            Vector3 p_max = max(max(p0, p1), p2);
            bboxes[i] = {BBox{p_min, p_max}, i};
        }
    }
    bvh_root_id = construct_bvh(bboxes, bvh_nodes);
}

std::optional<Intersection> intersect(const Scene &scene, const BVHNode &node, Ray ray) {
    if (node.primitive_id != -1) {
        return intersect(scene.shapes[node.primitive_id], ray);
    }
    const BVHNode &left = scene.bvh_nodes[node.left_node_id];
    const BVHNode &right = scene.bvh_nodes[node.right_node_id];
    std::optional<Intersection> isect_left;
    if (intersect(left.box, ray)) {
        isect_left = intersect(scene, left, ray);
        if (isect_left) {
            ray.tfar = isect_left->distance;
        }
    }
    if (intersect(right.box, ray)) {
        // Since we've already set ray.tfar to the left node
        // if we still hit something on the right, it's closer
        // and we should return that.
        if (auto isect_right = intersect(scene, right, ray)) {
            return isect_right;
        }
    }
    return isect_left;
}

bool occluded(const Scene &scene, const BVHNode &node, Ray ray) {
    if (node.primitive_id != -1) {
        return occluded(scene.shapes[node.primitive_id], ray);
    }
    const BVHNode &left = scene.bvh_nodes[node.left_node_id];
    const BVHNode &right = scene.bvh_nodes[node.right_node_id];
    if (intersect(left.box, ray)) {
        if (occluded(scene, left, ray)) {
            return true;
        }
    }
    if (intersect(right.box, ray)) {
        if (occluded(scene, right, ray)) {
            return true;
        }
    }
    return false;
}

std::optional<Intersection> intersect(const Scene &scene, Ray ray) {
    std::optional<Intersection> isect_left;
    return scene.bvh_nodes.size()? intersect(scene, scene.bvh_nodes[scene.bvh_root_id], ray) : isect_left;
}

bool occluded(const Scene &scene, const Ray &ray) {
    return occluded(scene, scene.bvh_nodes[scene.bvh_root_id], ray);
}

Vector3 evalDiffuse(const Scene &scene, MaterialBase mat, pcg32_state rng) {
    auto p = mat.rec->position;
    auto n = mat.rec->normal;
    bool ispointl = false;
    Real area = Real(0.);
    Real lightNum = scene.lights.size();
    Vector3 intensity, x, n_x = Vector3(0.,0.,0.);       
    // pcg32_state rng_ = init_pcg32(time(NULL)+1);
    auto r0 = next_pcg32_real<Real>(rng); //static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    auto l = scene.lights[floor(r0*lightNum)];
    if (auto *point_light = std::get_if<PointLight>(&l)) {
        x = point_light->position;
        ispointl = true;
        intensity = point_light->intensity;
    } else if (auto *area_light = std::get_if<AreaLight>(&l)) {
        auto lshape = scene.shapes[area_light->shape_id];
        if (auto sph = std::get_if<Sphere>(&lshape)) {
            x = sph->random(p, n_x, area, rng);
        } else if (auto tri = std::get_if<Triangle>(&lshape)) {
            x = tri->random(p, n_x, area, rng);
        }
        ispointl = false;
        intensity = area_light->radiance;
    }
    Vector3 tox = x - p;
    Vector3 wx = normalize(tox);
    Ray shadow_ray{p, wx, Real(1e-4), (1 - Real(1e-4)) * length(tox)};
    auto projected_intensed_color = lightNum * mat.getAlbedo() * intensity * max(dot(n,wx), Real(0)) / length_squared(tox) / c_PI;
    if (!occluded(scene, shadow_ray)) {
        return ispointl? projected_intensed_color : area * projected_intensed_color * max(dot(-n_x, wx), Real(0));
    }   else {
        return Vector3(0.,0.,0.);
    }
}