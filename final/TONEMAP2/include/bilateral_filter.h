#pragma once
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#define CHRONO
#include "linear_bf.h"
#include "image.h"

#ifndef __BILATERAL_FILTER__
#define __BILATERAL_FILTER__
typedef Array_2D<double> image_type;

void Image1toimage_type (Image1& img, image_type& image);

void image_typetoImage1 (image_type& image, Image1& img);

void bilateralFilter( Image1& img,
                      double sigma_s,
                      double sigma_r,
                      Image1& logF);

#endif