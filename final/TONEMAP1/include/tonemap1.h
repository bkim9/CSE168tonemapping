#ifndef __TONE1__
#define __TONE1__
#include "image.h"

Real Luminance(Vector3 v);

Real L_white_(Image3& img);

Real R_i(int x, int y, Real s, Real alpha_i);

Real conv(Image3& img, Real R, int x, int y);

Real right_s(Image3& img, int i, int j);

void tonemap1(Image3& img, Real ave_L, int x, int y);
#endif