#include "bilateral_filter.h"
#ifndef __TONE2__
#define __TONE2__
Real Luminance2(Vector3 v);

void LuminanceLayer(Image3& img, Image1& img1);

void logImg(Image3& img, Image1& logI);

Real maxI(Image1& img);

Real minI(Image1& img);

void tonemap2(Image3& img, double sigma_s, double sigma_r, double contrast) ;
#endif