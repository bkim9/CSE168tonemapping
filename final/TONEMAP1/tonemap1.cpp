#include "tonemap1.h"

Real ALPHA_1 = Real(.35);
Real EPSILON = Real(.1);
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

// i = 1 or 2
Real R_i(int i, int x_offset, int y_offset, Real s) {
    auto alpha_i = Real(.35) * pow(1.6, i-1); 
    auto as = alpha_i * s;
    auto x = x_offset;
    auto y = y_offset;
    return s==1? 1:1/(c_PI*as*as) * pow(2.7182818284, -(x*x + y*y)/(as*as)); 
}

// i = 1 or 2
// local average at (x,y)
Real V_i(Image3& img, int i, int x, int y, Real s) {
    auto wing = ceil(s * pow(1.6,i-1)/ 2.0);
    Real subTotal = Real(0); 
    // element wise multiplication
    for (int x_o = -wing; x_o <= wing; x_o++) {
    for (int y_o = -wing; y_o <= wing; y_o++) {
        if(0 <= x+x_o && x+x_o < img.width && 0 <= y+y_o && y+y_o < img.height && 2 * wing < img.width && 2 * wing < img.height) {
            subTotal += Luminance(img(x+x_o,y+y_o)) * R_i(i-1, x_o, y_o, s);
            subTotal /= s==1? 2.6:1; 
        }
    }
    }
    return subTotal;
}

Real V(Image3& img, Real s, int i, int j) { 
    /* key_value */
    Real a = .18;   
    /* free parameter */
    Real phi = 8.0;
    return (V_i(img,1,i,j,s) - V_i(img,2,i,j,s)) /
            (a *pow(2,phi)/(s*s) + V_i(img,1,i,j,s));
}

Real right_s(Image3& img, int i, int j) {
    Real s_temp = Real(1);
    Real s_ratio = Real(1.6);
    while (abs(V(img,s_temp,i,j)) >= EPSILON) s_temp *= s_ratio; 
    return s_temp;
}

// local tonemapping
void localtonemap1(Image3& img) {
    for( int i=0; i < img.width; i++) {
    for (int j=0; j< img.height; j++) {    
        auto s = Real(1.); // scale
        // find the right scale s for a pixel (x,y)
        s = right_s(img,i,j);
        // std::cout << "s: " << s << std::endl;
        img(i,j) /= (1 + V_i(img,1,i,j,s)); 
    }
    }
}

Real logAverageL(Image3& img) {
    Real log_sum_L, delta;
    delta= 0.00001;
    log_sum_L = Real(0);
    for( int i=0; i < img.width; i++) {
    for (int j=0; j< img.height; j++) {
        log_sum_L += log(delta + Luminance(img(i,j)));
    }
    }
    return pow(2.7182818284,log_sum_L) / (img.width * img.height);
}

// global tonemapping
void globaltonemap1(Image3& img) {
    auto log_ave_L = logAverageL(img);

    /* select a to be  .09 .18 .36 .45 .72 */
    auto a = Real(.18);                 
    auto L_white = L_white_(img) * a / log_ave_L;  

    // scale luminance
    for( int i=0; i < img.width; i++) {
    for (int j=0; j< img.height; j++) {    
        auto L_w = Luminance(img(i,j));
        auto L = a / log_ave_L * L_w; // scaled luminance
        img(i,j) *= L / L_w * (1 + L/L_white/L_white) / (1 + L);
    }
    }
}