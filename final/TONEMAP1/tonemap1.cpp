#include "tonemap1.h"

Real ALPHA_1 = Real(.35);
Real EPSILON = Real(.05);
Real MAX_L_D = Real(1.);

Real Luminance(Vector3 v) {
    return dot(Vector3(.27,.67,.06), v);
}

Real L_white_(Image3& img) {
    Real max_sofar = Luminance(img(0,0));
    for (int i =0; i < img.width; i++ ) {
    for (int j=0; j< img.height; j++ ) {
        max_sofar =fmax(max_sofar, Luminance(img(i,j)));
    }
    }
    return max_sofar;
}

Real R_i(int x, int y, Real s, Real alpha_i) {
    auto as = alpha_i * s;
    return 1/(c_PI*as*as) * pow(2.7182818284, -(x*x + y*y)/(as*as)); 
}


Real conv(Image3& img, Real R, int x, int y) {
    return Luminance(img(x,y)) * R;
}

Real V(Image3& img, Real s_temp, int i, int j) { 
    V_i(img,)
}

Real right_s(Image3& img, int i, int j) {
    Real s_temp = Real(1);
    Real s_ratio = Real(1.6);
    while (V(img,s_temp,i,j) >= EPSILON) s_temp *= ratio; 
    return s_temp;
}

void tonemap1(Image3& img, Real ave_L, int x, int y) {
    auto a = Real(.18); // select a to be  .09 .18 .36 .45 .72
    auto L_white = L_white_(img);  
    auto sharp = Real(1.);
    auto L_w = Luminance(img(x,y));
    auto L = a / ave_L * L_w; // scaled luminance
    auto s = Real(1.); // scale

    // find the right scale s for a pixel (i,j)
    s = right_s(img,i,j);



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