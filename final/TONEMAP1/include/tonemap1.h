#ifndef __TONE1__
#define __TONE1__
#include "image.h"

Real Luminance(Vector3 v);

Real L_white_(Image3& img);

Real R_i(int i, int x_offset, int y_offset, Real s);

Real V_i(Image3& img, int i, int x, int y, Real s);

Real V(Image3& img, Real s, int i, int j);

Real right_s(Image3& img, int i, int j);

void tonemap1(Image3& img, Real ave_L, int x, int y);
#endif