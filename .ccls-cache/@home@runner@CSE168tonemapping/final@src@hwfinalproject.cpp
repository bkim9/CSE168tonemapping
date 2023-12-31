#include "hwfinalproject.h"
Real ALPHA_1 = Real(.35);
Real ALPHA_2 = ALPHA_1 * Real(1.6);
// Real EPSILON = Real(.05);
Real MAX_L_D = Real(1.);

void materialsetup(Material* material, MaterialBase& mb) {    
    if (auto d = std::get_if<Diffuse>(material)) {
        mb.isdiffuse = true;
        mb.reflectance = d->getAlbedo();
        mb.texture = d->texture;
    } else if (auto m = std::get_if<Mirror>(material)) {
        mb.ismirror = true;
        mb.reflectance = m->getAlbedo();
        mb.texture = m->texture;
    } else if (auto p = std::get_if<Plastic>(material)) {
        mb.isplastic = true;
        mb.reflectance = p->getAlbedo();
        mb.texture = p->texture;
    } else if (auto ph = std::get_if<Phong>(material)) {
        mb.isphong = true;
        mb.reflectance = ph->getAlbedo();
        mb.texture = ph->texture;
    } else if (auto blinn = std::get_if<BlinnPhong>(material)) {
        mb.isblinn = true;
        mb.reflectance = blinn->getAlbedo();
        mb.texture = blinn->texture;
    } else if (auto micro = std::get_if<Mirror>(material)) {
        mb.ismicro = true;
        mb.reflectance = micro->getAlbedo();
        mb.texture = micro->texture;
    } 
}

pdf* sample_brdf(Material* mate, MaterialBase& mat, Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) {
    pdf* sampled_pdf = nullptr;
    if(auto diffuse = std::get_if<Diffuse>(mate)) {
        diffuse->rec = mat.rec;
        sampled_pdf = diffuse->sample_brdf(wi, wo, srec, rng); 
    } else if (auto mirror = std::get_if<Mirror>(mate)) {
        mirror->rec = mat.rec;
        sampled_pdf = mirror->sample_brdf(wi, wo, srec, rng);
    } if(auto plastic = std::get_if<Plastic>(mate)) {
        plastic->rec = mat.rec;
        sampled_pdf = plastic->sample_brdf(wi, wo, srec, rng);
    } else if (auto phong = std::get_if<Phong>(mate)) {
        phong->rec = mat.rec;
        sampled_pdf = phong->sample_brdf(wi, wo, srec, rng);
    } else if (auto blinn = std::get_if<BlinnPhong>(mate)) {
        blinn->rec = mat.rec;
        sampled_pdf = blinn->sample_brdf(wi, wo, srec, rng); 
    } else if (auto micro = std::get_if<BlinnPhongMicrofacet>(mate)) {
        micro->rec = mat.rec;
        sampled_pdf = micro->sample_brdf(wi, wo, srec, rng); 
    }
    return sampled_pdf;
}

Vector3 eval_brdf(Material* mate, MaterialBase& mat, Vector3 wi, Vector3 wo, pdf& pdf, bool nexthitlight) {
    Vector3 brdf_value{0.,0.,0.};
    if(auto diffuse = std::get_if<Diffuse>(mate)) {
        brdf_value = diffuse->eval_brdf(wi, wo, pdf, nexthitlight);
    } else if(auto plastic = std::get_if<Plastic>(mate)) {
        brdf_value = plastic->eval_brdf(wi, wo, pdf, nexthitlight);
    } else if (auto phong = std::get_if<Phong>(mate)) {
        brdf_value = phong->eval_brdf(wi, wo, pdf, nexthitlight);
    } else if (auto blinn = std::get_if<BlinnPhong>(mate)) { 
        brdf_value = blinn->eval_brdf(wi, wo, pdf, nexthitlight);
    } else if (auto micro = std::get_if<BlinnPhongMicrofacet>(mate)) {
        brdf_value = micro->eval_brdf(wi, wo, pdf, nexthitlight);
    }
    return brdf_value;
}

static Vector3 radiance(const Scene &scene, Ray ray, pcg32_state rng, int depth) {
    Vector3 L = Vector3{0, 0, 0};
    MaterialBase mat;
    Vector3 p, n, wi, wo; // the most important one is the wo
    mixed_pdf mix_pdf;
    scatter_record srec;
    if ( depth <= 0 ) return L;
    if (auto hit_isect = intersect(scene, ray)) {   
        auto mate = scene.materials[hit_isect->material_id];
        materialsetup(&mate, mat);
        mat.rec = hit_isect;
        p = hit_isect->position;
        n = hit_isect->normal;
        wi = -normalize(ray.dir);
        if (hit_isect->area_light_id != -1)  { 
            L += std::get<AreaLight>(scene.lights[hit_isect->area_light_id]).radiance;
        }
        mix_pdf.pdfs.push_back(sample_brdf(&mate, mat, wi, wo, srec, rng));
        Ray next_ray{p,wo,Real(.001),infinity<Real>()};
        auto sample_intersect = intersect(scene, next_ray);

        std::vector<pdf*> light_ps;
        for( auto l : scene.lights ) {
            auto p1 = new light_pdf;
            p1->setup(scene.shapes[std::get_if<AreaLight>(&l)->shape_id], ray, rng);
            light_ps.push_back(p1);
        }
        
        mix_pdf.setup(light_ps);
        
        if (mat.ismirror) {
            if (srec.sampling_weight > 0) {
                auto m = std::get_if<Mirror>(&mate);
                L += m->mirror_frasnel(wo, n)* 
                srec.sampling_weight * 
                radiance(scene, next_ray, rng, depth - 1);
            }
        } else {
            bool nexthitlight = sample_intersect && sample_intersect->area_light_id != -1;
            Real pdf_value = mix_pdf.value(wo,sample_intersect->position, nexthitlight); // 8.6580953675294765 -> 0.014474071966940219
            Vector3 brdf_value = eval_brdf(&mate, mat, wi, wo, mix_pdf, nexthitlight); 
            if ( brdf_value.x >= 0 && brdf_value.y >= 0 && brdf_value.z >= 0 && pdf_value > 0 ) {
                L += (brdf_value / pdf_value) * radiance(scene, next_ray, rng, depth - 1); 
            }
        }
        return L;
    }
    return scene.background_color;
}

Real Luminance(Vector3 v) {
    return dot(Vector3(.27,.67,.06), v);
}
Real R_i(int x, int y, Real s, int i) {
    auto alpha = i = 1? ALPHA_1: ALPHA_2;
    return pow(2.7182818284, -(x*x + y*y)/pow(alpha,2)); 
}

Real conv(Image3& img, Real R, int x, int y) {
    return Luminance(img(x,y)) * R;
}

void tonemap1(Image3& img, Real ave_L, int x, int y) {
    auto a = Real(.18); // select a to be  .09 .18 .36 .45 .72
    auto L_white = Real(10.); 
    auto sharp = Real(1.);
    auto L_w = Luminance(img(x,y));
    auto L = a / ave_L * L_w; // scaled luminance
    auto s = Real(1.); // scale
    auto R_1 = R_i(x,y,s,1);
    auto R_2 = R_i(x,y,s,2);

    auto V_1 = conv(img,R_1, x, y);
    auto V_2 = conv(img,R_2, x, y);
    
    Real V = Real(0.0);
    do{
        V = (V_1 - V_2) / 
             (a * pow(2,sharp)/(s*s)+V_1);
        s *= Real(1.6);
        if( s > Real(43)) std::cout << "s too big: " << s << std::endl;
    } while (fabs(V) >= EPSILON);
    auto L_d = L *(1 + L/(L_white * L_white)) /(1+V);
    img(x,y) *= L_d;
}

// 
// Implementation of finding Luminanance
// 
Image3 hw_fin_img(const std::vector<std::string> &params) {
    if (params.size() < 1) {
        return Image3(0, 0);
    }

    int max_depth = 5;
    std::string filename;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-max_depth") {
            max_depth = std::stoi(params[++i]);
        } else if (filename.empty()) {
            filename = params[i];
        }
    }
    Timer timer;
    tick(timer);
    ParsedScene parsed_scene = parse_scene(filename);    
    std::cout << "Scene parsing done. Took " << tick(timer) << " seconds." << std::endl;

    tick(timer);
    Scene scene(parsed_scene);
    std::cout << "Scene construction done. Took " << tick(timer) << " seconds." << std::endl;
    int spp = scene.samples_per_pixel;

    Image3 img(scene.width, scene.height);

    int w = img.width;
    int h = img.height;

    CameraRayData cam_ray_data = compute_camera_ray_data(
        scene.camera, w, h);

    constexpr int tile_size = 16;
    int num_tiles_x = (w + tile_size - 1) / tile_size;
    int num_tiles_y = (h + tile_size - 1) / tile_size;

    ProgressReporter reporter(num_tiles_x * num_tiles_y);
    tick(timer);

    parallel_for([&](const Vector2i &tile) {
        // Use a different rng stream for each thread.
        pcg32_state rng = init_pcg32(tile[1] * num_tiles_x + tile[0]);
        int x0 = tile[0] * tile_size;
        int x1 = min(x0 + tile_size, w);
        int y0 = tile[1] * tile_size;
        int y1 = min(y0 + tile_size, h);
        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                for (int s = 0; s < spp; s++) {
                    Real u, v;
                    u = (x + next_pcg32_real<Real>(rng)) / w;
                    v = (y + next_pcg32_real<Real>(rng)) / h;
                    Ray ray = generate_primary_ray(cam_ray_data, u, v);
                    img(x, y) += radiance(scene, ray, rng, max_depth);
                }
                img(x,y) /= Real(spp);
            }
        }

        reporter.update(1);
    }, Vector2i(num_tiles_x, num_tiles_y));



    reporter.done();
    std::cout << "Rendering done. Took " << tick(timer) << " seconds." << std::endl;

    return img;
}

Image3 hw_fin_1(const std::vector<std::string> &params) {
    Image3 img = hw_fin_img(params);

    // log_sum_L
    Real log_sum_L, delta;
    delta= 0.00001;
    log_sum_L = Real(0);
    for( int i=0; i < img.width; i++) {
        for (int j=0; j< img.height; j++) {
            auto imgL = Luminance(img(i,j));
            log_sum_L += log(delta + imgL);
        }
    }

    auto log_ave_L = pow(10,log_sum_L) / (img.width * img.height);
    for( int i=0; i < img.width; i++) {
        for (int j=0; j< img.height; j++) {
            tonemap1(img,log_ave_L, i, j);
        }
    }
    return img;
}

// bilateral filter tone mapping
Image3 hw_fin_2(const std::vector<std::string> &params) {
    Image3 img = hw_fin_img(params);
    double sigma_s = 16.0;
    double sigma_r = 0.1;
    colorBilateralfilter(img, sigma_s, sigma_r);
    return img;
}

// logmap
Image3 hw_fin_3(const std::vector<std::string> &params) {
    Image3 img = hw_fin_img(params);
    log_tone( img );
    return img;
}
