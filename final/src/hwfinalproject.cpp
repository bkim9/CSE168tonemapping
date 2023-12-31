#include "hwfinalproject.h"
void exportimg(Image3& img, std::string filename) {
    std::ofstream myfile;
    myfile.open(filename);
    std::cout << "Writing this to a file:\n" << filename << std::endl;
    
    myfile << img.width << ' ' << img.height << ' ';
    for( int i = 0; i < img.width; i++ ) {
    for (int j = 0; j < img.height;j++ ) {
        myfile << img(i,j).x << ' '<< img(i,j).y << ' ' << img (i,j).z << ' ';
    }    
    }
    myfile.close();
    return;
}

Image3 importimg(const char* filename) {
  FILE *infp;
  infp = fopen(filename, "r");

  int width = 0;
  int height= 0;

  fscanf(infp,"%d",&width);
  fscanf(infp,"%d",&height);

  Image3 img(width,height);

  for( int i = 0; i < img.width; i++ ) {
  for (int j = 0; j < img.height;j++ ) {
    fscanf(infp,"%lf",&img(i,j).x);
    fscanf(infp,"%lf",&img(i,j).y);
    fscanf(infp,"%lf",&img(i,j).z);
  }    
  }
  std::cout<< img(10, 10) << std::endl;
  return img;
}

Vector3 averageV(Image3& img, int i, int j) {
    int start_x = i-1;
    int end_x   = i+1;
    int start_y = j-1;
    int end_y   = j+1;
    int count = 0;
    Vector3 subtotal(0,0,0);
    if( i == 0 ) start_x = i;
    if( j == 0 ) start_y = j;
    if( i == img.width -1 ) end_x = i;
    if( j == img.height-1 ) end_y = j;
    std::cout<< std::endl<< std::endl;
    for( int x = start_x; x <= end_x; x++ ) {
    for( int y = start_y; y <= end_y; y++ ) {
        if( x != i || y != j ){
            std::cout << img(x,y) << '\t';
            subtotal += img(x,y);
            count++;
        } else {
            std::cout <<"  ___center___  " ;
        }
    }
    std::cout << std::endl;
    }
    auto averageV_ = subtotal / Real(count);
    return averageV_;
}

void smooth(Image3& img) {
    for( int i = 0; i< img.width; i++ ){
    for( int j = 0; j< img.height; j++) {
        auto ave = averageV(img, i, j);
        auto diff = length(img(i,j) - ave);
        if( diff * 2.0 > length(ave)) 
            img(i,j) = ave;
    }
    }
}

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

pdf* sample_pdf(Material* mate, MaterialBase& mat, Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) {
    pdf* sampled_pdf = nullptr;
    if(auto diffuse = std::get_if<Diffuse>(mate)) {
        diffuse->rec = mat.rec;
        sampled_pdf = diffuse->sample_pdf(wi, wo, srec, rng); 
    } else if (auto mirror = std::get_if<Mirror>(mate)) {
        mirror->rec = mat.rec;
        sampled_pdf = mirror->sample_pdf(wi, wo, srec, rng);
    } if(auto plastic = std::get_if<Plastic>(mate)) {
        plastic->rec = mat.rec;
        sampled_pdf = plastic->sample_pdf(wi, wo, srec, rng);
    } else if (auto phong = std::get_if<Phong>(mate)) {
        phong->rec = mat.rec;
        sampled_pdf = phong->sample_pdf(wi, wo, srec, rng);
    } else if (auto blinn = std::get_if<BlinnPhong>(mate)) {
        blinn->rec = mat.rec;
        sampled_pdf = blinn->sample_pdf(wi, wo, srec, rng); 
    } else if (auto micro = std::get_if<BlinnPhongMicrofacet>(mate)) {
        micro->rec = mat.rec;
        sampled_pdf = micro->sample_pdf(wi, wo, srec, rng); 
    }
    return sampled_pdf;
}

Vector3 eval_brdf(Material* mate, MaterialBase& mat, Vector3 wi, Vector3 wo, pdf& pdf) {
    Vector3 brdf_value{0.,0.,0.};
    if(auto diffuse = std::get_if<Diffuse>(mate)) {
        brdf_value = diffuse->eval_brdf(wi, wo, pdf);
    } else if(auto plastic = std::get_if<Plastic>(mate)) {
        brdf_value = plastic->eval_brdf(wi, wo, pdf);
    } else if (auto phong = std::get_if<Phong>(mate)) {
        brdf_value = phong->eval_brdf(wi, wo, pdf);
    } else if (auto blinn = std::get_if<BlinnPhong>(mate)) { 
        brdf_value = blinn->eval_brdf(wi, wo, pdf);
    } else if (auto micro = std::get_if<BlinnPhongMicrofacet>(mate)) {
        brdf_value = micro->eval_brdf(wi, wo, pdf);
    }
    return brdf_value;
}

Vector3 radiance(const Scene &scene, Ray ray, pcg32_state rng, int depth) {
    Vector3 L = Vector3{0, 0, 0};
    MaterialBase mat;
    Vector3 p, n, wi, wo; // the most important one is the wo
    mixed_pdf mix_pdf;
    scatter_record srec;
    if ( depth <= 0 ) return L;
    if (auto hit_isect = intersect(scene, ray)) {   
        auto mate = scene.materials[hit_isect->material_id];
        mat.rec = hit_isect;
        materialsetup(&mate, mat);  
        p = hit_isect->position;
        n = hit_isect->normal;
        wi = -normalize(ray.dir);
        if (hit_isect->area_light_id != -1)  { 
            L += std::get<AreaLight>(scene.lights[hit_isect->area_light_id]).radiance;
        }
        int dice = floor(scene.lights.size() * next_pcg32_real<Real>(rng));
        mix_pdf.mat_pdf = sample_pdf(&mate, mat, wi, wo, srec, rng);
        if (std::get_if<AreaLight>(&scene.lights[dice])) {
            std::vector<light_pdf*> light_ps;
            for( auto l : scene.lights ) {
                if(auto al = std::get_if<AreaLight>(&l)) {
                    auto lp = new light_pdf;
                    lp->setup(scene.shapes[al->shape_id], p, rng);
                    light_ps.push_back(lp);
                } 
            }
            mix_pdf.setup(light_ps);
            wo = mix_pdf.generate();
            Ray next_ray{p,wo,Real(.001),infinity<Real>()};
            auto sample_intersect = intersect(scene, next_ray);

            if (mat.ismirror) {
                if (srec.sampling_weight > 0) {
                    wo = mix_pdf.pdfs[0]->generate();
                    auto m = std::get_if<Mirror>(&mate);
                    L += m->mirror_frasnel(wo, n)* 
                    srec.sampling_weight * 
                    radiance(scene, next_ray, rng, depth - 1);
                }
            } else {
                // bool nexthitlight = sample_intersect && sample_intersect->area_light_id != -1;
                Real pdf_value = mix_pdf.value(wo); // 8.6580953675294765 -> 0.014474071966940219
            // std::cout << pdf_value << std::endl;
                Vector3 brdf_value = eval_brdf(&mate, mat, wi, wo, mix_pdf); 
                if ( brdf_value.x >= 0 && brdf_value.y >= 0 && brdf_value.z >= 0 && pdf_value > 0 ) {
                    L += (brdf_value / pdf_value) * radiance(scene, next_ray, rng, depth - 1); 
                }
            }
        } else if (auto pl = std::get_if<PointLight>(&scene.lights[dice])) {
            wo = normalize(pl->position - p);
            // Ray next_ray{p,wo,Real(.001),infinity<Real>()};
            Real pdf_value = mix_pdf.value(wo);

            Vector3 brdf_value = eval_brdf(&mate, mat, wi, wo, mix_pdf); 
            if (mat.ismirror) {
                auto m = std::get_if<Mirror>(&mate);
                L += m->mirror_frasnel(wo, n)* 
                pl->intensity;
            } else {                    
                Vector3 l = pl->position - p;
                Ray shadow_ray{p, normalize(l), Real(1e-4), (1 - Real(1e-4)) * length(l)};
                if ( !occluded(scene, shadow_ray) && brdf_value.x >= 0 && brdf_value.y >= 0 && brdf_value.z >= 0 && pdf_value > 0 ) {
                    L += (brdf_value / pdf_value) * pl->intensity/distance_squared(pl->position, p) * fmax(0,dot(n,wo)); 
                }
            }
        }
        return L;
    }
    return scene.background_color;
}

// 
// Implementation of finding Luminanance
// 
Image3 hw_fin_img(const std::vector<std::string> &params) {
    if (params.size() < 1) {
        return Image3(0, 0);
    }

    int max_depth = 7;
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
    int seed_ground = time(NULL);
    parallel_for([&](const Vector2i &tile) {
        // Use a different rng stream for each thread.
        // pcg32_state rng = init_pcg32(tile[1] * num_tiles_x + tile[0]);
        int x0 = tile[0] * tile_size;
        int x1 = std::min(x0 + tile_size, w);
        int y0 = tile[1] * tile_size;
        int y1 = std::min(y0 + tile_size, h);
        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                for (int s = 0; s < spp; s++) {
                    Real u, v;
                    int seed = seed_ground + tile[0] + 
                                    num_tiles_x * tile[1] + 
                                    num_tiles_x * num_tiles_y * s + 
                                    num_tiles_x * num_tiles_y * spp * x + 
                                    num_tiles_x * num_tiles_y * spp * x1 * y; 
                    pcg32_state rng = init_pcg32(seed);
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
    auto logAve = logAverageL(img);
    if ( logAve != Real(0) && logAve != infinity<Real>() ) globaltonemap1(img);
    localtonemap1(img);
    return img;
}

// bilateral filter tone mapping
Image3 hw_fin_2(const std::vector<std::string> &params) {
    Image3 img = hw_fin_img(params);
    double space_sigma = 0.02 * min(img.width,img.height);
    double range_sigma = 0.4;
    double contrast = 50.;
    tonemap2(img, space_sigma, range_sigma, contrast);
    return img;
}

// logmap
Image3 hw_fin_3(const std::vector<std::string> &params) {
    Image3 img = hw_fin_img(params);
    // log_tone( img );
    std::string filename = "ori/sponza.txt";
    exportimg(img, filename);
    return img;
}

Image3 hw_fin_4(const std::vector<std::string> &params) {
    int N = 8;
    Image3 imgtemp = importimg(("./ori/dining_" + std::to_string(0) + ".txt").c_str());
    Image3 img(imgtemp.width, imgtemp.height);
    for( int t = 0; t < N; t++) { 
        imgtemp = importimg(("./ori/dining_" + std::to_string(t) + ".txt").c_str());
        for (int i=0; i < img.width ; i++){
        for (int j=0; j < img.height; j++){
            img(i,j) += imgtemp(i,j) / Real(N);
        }}    
    }
    return img;
}
