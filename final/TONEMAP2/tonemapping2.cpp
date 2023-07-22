#include "tonemapping2.h"

Real Luminance2(Vector3 v) {
    return dot(Vector3(.27,.67,.06), v);
}

void LuminanceLayer(Image3& img, Image1& img1) {
    for( int i =0; i < img.width; i++) {
    for( int j =0; j < img.height; j++) {
        img1(i,j) = Luminance2(img(i,j));
    }
    }
}

void logImg(Image3& img, Image1& logI) {
    for( int i = 0; i< img.width; i++ ) {
    for( int j = 0; j< img.height; j++) {
        logI(i,j) = log( .0001 + Luminance2(img(i,j))) / log(10.0);
    }
    }
}

Real maxI(Image1& img) {
    Real max = img(0,0);
    for( int i =0; i < img.width; i++) {
    for( int j =0; j < img.height; j++) {
        max = max < img(i,j)? img(i,j) : max;
    }
    }
    return max;
}

Real minI(Image1& img) {
    Real min = img(0,0);
    for( int i =0; i < img.width; i++) {
    for( int j =0; j < img.height; j++) {
        min = min > img(i,j)? img(i,j) : min;
    }
    }
    return min;
}

void tonemap2(Image3& img, double sigma_s, double sigma_r, double contrast) {
    int w = img.width;
    int h = img.height;
    Image1 logI(w,h); 
    logImg(img,logI);
    Image1 logF(w,h);

    // (d) filter log(I) using the bilateral filter to get log(F)
    bilateralFilter(logI,sigma_s, sigma_r, logF);

    // (e) compute a detail channel D = log(I) - log(F)
    Image1 D(w,h);
    for( int i =0; i < w; i++) {
    for( int j =0; j < h; j++) {
        D(i,j) = logI(i,j) - logF(i,j);
    }
    }

    // (f) compute delta = max[log(F)] - min [log(F)]
    Real delta = maxI(logF) - minI(logF);
    
    // (g) compute gamma = log(contrast) / delta
    Real gamma = log(contrast) / log(10.) / delta;

    // (h) compute the new intensity layter N = 10^{gamma * logF + D}
    Image1 N(w,h);
    for( int i =0; i < w; i++) {
    for( int j =0; j < h; j++) {
        N(i,j) = pow(10.,gamma * logF(i,j) + D(i,j));
    }
    }

    Image1 I(w,h);
    LuminanceLayer(img,I);
    // (i) scale the img by N/I
    for( int i =0; i < w; i++) {
    for( int j =0; j < h; j++) {
        img(i,j) *= N(i,j) / I(i,j);
    }
    }
}