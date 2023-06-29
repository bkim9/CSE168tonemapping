#pragma once
#include "torrey.h"
#include "vector.h"
#include "pcg.h"

class Perlin {
    public:
        Perlin() {}
        Perlin(pcg32_state rng) {
            ranfloat = new double[point_count];
            perlin_rng = rng;
            for ( int i = 0; i < point_count; i++ ) {
                ranfloat[i] = next_pcg32_real<Real>(perlin_rng);
            }
            perm_x = perlin_generate_perm();
            perm_y = perlin_generate_perm();
            perm_z = perlin_generate_perm();
        }

        ~Perlin() {
            delete[] ranfloat;
            delete[] perm_x;
            delete[] perm_y;
            delete[] perm_z;
        }

        double noise(const Vector3& p) const {
            auto i = static_cast<int>(4*p.x) & 255;
            auto j = static_cast<int>(4*p.y) & 255;
            auto k = static_cast<int>(4*p.z) & 255;

            return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
        }

    private:
        pcg32_state perlin_rng;
        static const int point_count = 256;
        double* ranfloat;
        int* perm_x;
        int* perm_y;
        int* perm_z;

        int* perlin_generate_perm() {
            auto p = new int[point_count];

            for( int i=  0; i < Perlin::point_count; i++ ) 
                p[i] = i;
            
            permute(p, point_count);

            return p;
        } 

        void permute(int* p, int n) {
            for (int i= n-1; i>0; i--) {
                int target = randomint(Real(0/1.0), Real(i/1.0));
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }

        int randomint(Real start, Real end) {
            return floor(start + (end - start)* next_pcg32_real<Real>( Perlin::perlin_rng));
        }
};