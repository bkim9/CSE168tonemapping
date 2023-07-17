#pragma once
#include "shape.h"

class pdf {
    public:
        pcg32_state rng;
        onb uvw;
        virtual ~pdf() {}
        virtual double value(const Vector3& direction) const = 0;
        virtual Vector3 generate() const = 0;
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

        virtual Vector3 generate() const override {
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

        virtual Vector3 generate() const override {
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

        virtual Vector3 generate() const override {
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

        virtual Vector3 generate() const override{
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
            return sph? sph->pdf_value(p,direction):  // May 25th solved by putting islighthit x = 2.1222944121466691E-314, y = 5.215016859923639E-310, z = 5.215016859923639E-310
                        tri->pdf_value(p,direction);
        }

        // 
        //  shape |
        //   - x -
        //     \.
        //      (wo)   /
        //          p
        virtual Vector3 generate() const override {
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
        std::vector<pdf*> pdfs;
        mixed_pdf() {
            pdfs.clear();
        }

        void setup(std::vector<pdf*> pdfvec) {
            for( auto lightp: pdfvec) {
                pdfs.push_back(lightp);
            }
        }

        virtual double value(const Vector3& direction) const override {
            auto N = pdfs.size();
            if (N==0) return 1.0;
            Real sum = 0;
            auto matpdf_value = pdfs[0]->value(direction);
            for( int i = 1; i < N; i++) {
                sum += pdfs[i]->value(direction); 
            }
            return (N==1) ? matpdf_value : (matpdf_value + sum / (N-1))
                                                         / 2.;
        }

        virtual Vector3 generate() const override {
            auto r0 = random_real(rng);
            auto N = pdfs.size();
            int r1 = floor(2. * (N - 1) * r0 - (N - 2));
            int targetI = r0 < .5 || r0 == 1? 0 : r1;
            return pdfs[targetI]->generate();
        }
};