#include "torrey.h"
#include "parallel.h"
class onb {
    public:
        Vector3 u;
        Vector3 v;
        Vector3 w;
        onb(){}
        onb(const Vector3 &n) {
            build_from_w(n);
        }

        void build_from_w (const Vector3 &n) {
            w = normalize(n);
            Vector3 a = fabs(w.y) > .9 ? Vector3(0.,0.,1.) : Vector3(0.,1.,0.);
            v = normalize(cross(w,a));
            u = cross(v,w);
        }
        
        Vector3 local(Real a, Real b, Real c) const {
            return a * u + b * v + c * w;
        }

        Vector3 local(Vector3 a) const {
            return local(a.x,a.y,a.z);
        }
};