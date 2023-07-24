#pragma once
#include "shape.h"

class pdf {
    public:
        pcg32_state rng;
        onb uvw;
        virtual ~pdf() {}
        virtual double value(const Vector3& direction) const = 0;
        virtual Vector3 generate() = 0;
};

// cosine hemisphere sampling using cosine_pdf = cos(theta) / c_PI
inline Vector3 random_cosine_direction(pcg32_state rng) {
    
    auto r1 = next_pcg32_real<Real>(rng);
    auto r2 = next_pcg32_real<Real>(rng);
    return hemiloc(r1, sqrt(1-r2));
}

// phong sampling using phong_pdf = cos(theta)^alpha (alpha + 1) / (2 * c_PI)
inline Vector3 random_phong_direction(Real exponent, pcg32_state rng) {
    auto r1 = next_pcg32_real<Real>(rng);
    auto r2 = next_pcg32_real<Real>(rng);
    return hemiloc(r1, pow(1-r2,1/(exponent+1)));
}


inline Real random_real(pcg32_state rng) {
    return next_pcg32_real<Real>(rng);
}

class cosine_pdf : public pdf {
    public:
        cosine_pdf(pdf* samplepdf) {
            rng = samplepdf->rng;
            uvw.build_from_w(samplepdf->uvw.w);
        }
        cosine_pdf(const Vector3& n, pcg32_state rang) { uvw.build_from_w(n); rng = rang;}

        // direction: wo
        virtual double value(const Vector3& direction) const override {
            auto cosine = dot(normalize(direction), uvw.w); // cosine: 0.65158585123057444
            return cosine <= 0? 0 : cosine / c_PI;
        }

        virtual Vector3 generate() override {
            Vector3 cosine_sampled_wo = uvw.local(normalize(random_cosine_direction(rng)));
            return r0 < F? r : cosine_sampled_wo;
        }
        Real r0=0;
        Vector3 r;
        Real F=0;
};

class phong_pdf : public pdf {
    public:
        phong_pdf(pdf* samplepdf) {
            rng = samplepdf->rng;
            uvw.build_from_w(samplepdf->uvw.w);
            alpha = 1.;
        }
        phong_pdf(const Vector3& r, Real exponent, pcg32_state rang) { uvw.build_from_w(r); alpha = exponent; rng = rang;}
            
        virtual double value(const Vector3& direction) const override {
            auto cosine = dot(normalize(direction), uvw.w);
            return (cosine <= 0)? 0 : pow(cosine,alpha) * (alpha + 1) / (2 * c_PI);
        }

        virtual Vector3 generate() override {
            return uvw.local(random_phong_direction(alpha, rng)); // how to handle when the direction is below the surface
        }
        Real alpha;
};

class blinn_pdf : public pdf {
    public:
        blinn_pdf(const Vector3& ns, Real exponent, pcg32_state rang) { uvw.build_from_w(ns); alpha = exponent; rng = rang;}
        
        virtual double value(const Vector3& wo) const override {
            auto cosine = dot(uvw.w, normalize(h));
            auto jacobian = Real(1.) / (4. * dot(normalize(wo), normalize(h)));
            return (cosine <= 0)? 0 : (alpha + 1) / (2 * c_PI) * pow(cosine,alpha) * jacobian;
        }
        // ** set h after generate()
        // generate h not wo 
        virtual Vector3 generate_h() const {
            return uvw.local(random_phong_direction(alpha, rng));
        }

        virtual Vector3 generate() override {
            return  2. * dot(h,wi) * h  - wi;
        }

    public:
        Vector3 h;
        Vector3 wi;
        Real alpha;
};

class micro_pdf : public pdf {
    public:
        micro_pdf(const Vector3& ns, Real exponent, pcg32_state rang) { uvw.build_from_w(ns); alpha = exponent; rng = rang;}
        
        virtual double value(const Vector3& wo) const override {
            auto cosine = dot(uvw.w, normalize(h));
            auto jacobian = Real(1.) / (4. * dot(normalize(wo), normalize(h)));
            return (cosine <= 0)? 0 : (alpha + 1) / (2 * c_PI) * pow(cosine,alpha) * jacobian;
        }
        // ** set h after generate()
        // generate h not wo 
        virtual Vector3 generate_h() const {
            return uvw.local(random_phong_direction(alpha, rng));
        }

        virtual Vector3 generate() override{
            return 2. * dot(h, wi)* h  - wi;
        }

    public:
        Vector3 wi;
        Vector3 h;
        Real alpha;
};

class light_pdf : public pdf {
    public:
        light_pdf (){}
        bool setup (const Shape s, Vector3 shadingpoint, pcg32_state rang) { 
            sph = std::get_if<Sphere>(&s);
            tri = std::get_if<Triangle>(&s);
            p = shadingpoint;
            rng = rang;
            return true;
        }
        
        // direction: wo
        virtual double value(const Vector3& direction) const override {
            // do they meet or no
            std::__1::optional<Intersection> hit;
            double pdfV= 0.0;
            if ( sph ) {
                hit = intersect(*sph, Ray{p, direction, 0.00001, infinity<Real>()});
                pdfV = sph->pdf_value(p,direction);
            } else if (tri) {
                hit = intersect(*tri, Ray{p, direction, 0.00001, infinity<Real>()});
                pdfV = tri->pdf_value(p,direction);
            }
            return hit? pdfV: 0.0;  // May 25th solved by putting islighthit x = 2.1222944121466691E-314, y = 5.215016859923639E-310, z = 5.215016859923639E-310
                        
        }

        // 
        //  shape |
        //   - x -
        //     \.
        //      (wo)   /
        //          p
        virtual Vector3 generate() override {
            Real dummyReal = Real(0);
            Vector3 dummyN{0,0,0};
            return sph? normalize(sph->random(p, dummyN, dummyReal, rng) - p): 
                        normalize(tri->random(p, dummyN, dummyReal, rng) - p);
        };

        const Sphere* sph;
        const Triangle* tri;
        Vector3 p;
};

class mixed_pdf : public pdf {
    public:
        pdf* mat_pdf;
        std::vector<light_pdf*> pdfs;
        int targetI;
        mixed_pdf() {
            pdfs.clear();
            targetI = -1;
        }

        void setup(std::vector<light_pdf*> pdfvec) {
            for( auto lightp: pdfvec) {
                pdfs.push_back(lightp);
            }
        }

        virtual double value(const Vector3& direction) const override {
            auto N = pdfs.size();
            auto matpdf_value = mat_pdf->value(direction);
            auto ligpdf_value = targetI == -1 ? matpdf_value : pdfs[targetI]->value(direction); 
            
            return (matpdf_value + ligpdf_value) / 2.;
        }

        virtual Vector3 generate() override {
            auto r0 = random_real(rng);
            auto N = pdfs.size();
            if (r0 < .5 || r0 == 1) {
                return mat_pdf->generate();
            } else {
                r0 = 2*r0 -1;
                targetI = floor(N * r0);
                return pdfs[targetI]->generate();
            }
        }
};