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

// i = 0 or 1
Real R_i(int i, int x_offset, int y_offset, Real s) {
    auto alpha_i = Real(.35) * pow(1.6, i); 
    auto as = alpha_i * s;
    auto x = x_offset;
    auto y = y_offset;
    return 1/(c_PI*as*as) * pow(2.7182818284, -(x*x + y*y)/(as*as)); 
}


// local average at (x,y)
Real V_i(Image3& img, int i, int x, int y, Real s) {
    auto wing = ceil(s);
    Real subTotal = Real(0);
    // element wise multiplication
    for (int x_o = -wing; x_o < wing; x_o++) {
    for (int y_o = -wing; y_o < wing; y_o++) {
        if(0 <= x+x_o && x+x_o < img.width && 0 <= y+y_o && y+y_o < img.height) {
            subTotal += Luminance(img(x+x_o,y+y_o)) * R_i(i, x_o, y_o, s);
        } 
    }
    }
    return subTotal;
}

Real V(Image3& img, Real s, int i, int j) { 
    Real a = .18;   // key_value
    Real phi = 8.0; // free parameter
    return (V_i(img,1,i,j,s) - V_i(img,0,i,j,s)) /
            (a *pow(2,phi)/(s*s) + V_i(img,1,i,j,s));
}

Real right_s(Image3& img, int i, int j) {
    Real s_temp = Real(1);
    Real s_ratio = Real(1.6);
    while (abs(V(img,s_temp,i,j)) >= EPSILON) s_temp *= s_ratio; 
    return s_temp;
}

// local tonemapping
void tonemap1(Image3& img, Real ave_L, int x, int y) {
    auto a = Real(.18); // select a to be  .09 .18 .36 .45 .72
    auto L_white = L_white_(img);  
    auto sharp = Real(1.);
    auto L_w = Luminance(img(x,y));
    auto L = a / ave_L * L_w; // scaled luminance
    auto s = Real(1.); // scale
    // find the right scale s for a pixel (x,y)
    s = right_s(img,x,y);
    std::cout << "s: " << s << std::endl;
    img(x,y) /= (1 + V_i(img,1,x,y,s));
}