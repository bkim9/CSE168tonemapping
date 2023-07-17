#pragma once
#include "pdf.h"
#include "ray.h"
#include "torrey.h"
#include "vector.h"
#include <variant>
#include "Perlin.h"
#include "parse_scene.h"
#include "image.h"
#include "light.h"

struct Diffuse;
struct Mirror;
struct Plastic;
struct Phong;
struct BlinnPhong;
struct BlinnPhongMicrofacet;


struct scatter_record {
    Ray specular_ray;
    bool is_specular, is_plasic, is_phong, is_blinn, is_micro;
    Vector3 attenuation;
    pdf* pdf_ptr;
    Real sampling_weight;
};

struct Texture {
    Image3 textureImg;
    Perlin noise;
    double uscale;
    double vscale;
    double uoffset;
    double voffset;

    Texture ( const ParsedImageTexture* p) {        
        uscale = p->uscale;
        vscale = p->vscale;
        uoffset = p->uoffset;
        voffset = p->voffset;
        pcg32_state rng = init_pcg32(0);
        Perlin noise = Perlin(rng);
        if(p->isnoise) {
            noise_image();
        } else {
            textureImg = imread3(p->filename);
        }
    }

    virtual Vector3 value (const Vector3& p) {
        return Vector3(1,1,1) * noise.noise(p);
    }

    Image3 noise_image() {
        for( int i= 0; i < static_cast<int>(uscale); i++) {
            for( int j= 0; j < static_cast<int>(vscale); j++) {
                Vector3 p = Vector3(Real(0),Real(i),Real(j));
                textureImg(i,j) = value(p);
            }   
        }
        return textureImg;
    }   
};


struct MaterialBase {
    Vector3 reflectance;
    Texture* texture;
    std::optional<Intersection> rec;
    bool isdiffuse=false;
    bool ismirror=false;
    bool isplastic = false;
    bool isphong= false; 
    bool isblinn= false;
    bool ismicro= false;
    Vector3 getAlbedo() {
        Vector3 albedo = reflectance;
        if (texture) {
            double imageX, imageY;
            imageX = texture->textureImg.width * modulo(texture->uscale * rec->u + texture->uoffset,1.0 );
            imageY = texture->textureImg.height * modulo(texture->vscale * rec->v + texture->voffset,1.0 );
            albedo = texture->textureImg(imageX,imageY);
        }
        return albedo;
    }

    virtual pdf* sample_pdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng);
    virtual Vector3 eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf);

    virtual Vector3 eval_frasnel(Vector3 w, Vector3 n) {
        auto F0 = getAlbedo();
        return F0 + (1 - F0) * pow(1-dot(n,normalize(w)),5 );
    }

    Vector3 mirror_frasnel(Vector3 refl_raydir, Vector3 n) {
        return getAlbedo() + (1-getAlbedo()) * pow(1 - dot(n, normalize(refl_raydir)),5);
    }
};



struct Diffuse : public MaterialBase {
    Diffuse() {}
    ~Diffuse() {}
    virtual pdf* sample_pdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) override;

    virtual Vector3 eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf) override;
};

struct Mirror : public MaterialBase { 
    virtual pdf* sample_pdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) override;

};

struct Plastic : public MaterialBase {
    Real eta;

    virtual pdf* sample_pdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) override;
    virtual Vector3 eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf) override;

    Real plastic_frasnel(Vector3 wi) {
        auto n = normalize(rec->normal);
        auto F0 = pow((eta-1)/(eta+1),2);
        return F0 + (1 - F0) * pow(1-dot(n,wi),5 );
    }
};

struct Phong : public MaterialBase {
    Real exponent;
    virtual pdf* sample_pdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) override ;
    virtual Vector3 eval_brdf(Vector3 wi, Vector3 wo, pdf& pdf) override;
};

struct BlinnPhong : public MaterialBase {
    Real exponent;
    virtual pdf* sample_pdf(Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) override;
    virtual Vector3 eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf) override;
};

struct BlinnPhongMicrofacet : public MaterialBase {
    Real exponent;
    virtual pdf* sample_pdf(Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) override;
    virtual Vector3 eval_brdf(Vector3 wi, Vector3 wo, pdf& pdf) override;
    Real D(Vector3 n, Vector3 h) {
        return (exponent + 1) / (2. * c_PI) * pow(dot(n,h),exponent);
    }
    Real G1_hat(Vector3 n, Vector3 v) {
        auto tan_theata = sqrt(1/pow(dot(n,v),2)-1);
        auto a = sqrt(exponent/2. + 1)/ tan_theata; 
        return a >= 1.6? 1. : (3.535 * a + 2.181 * a * a) /
                            (1 + 2.276 * a + 2.577 * a * a);
    }
};


using Material = std::variant<Diffuse, Mirror, Plastic, Phong, BlinnPhong, BlinnPhongMicrofacet>;