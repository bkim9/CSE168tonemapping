#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include "geom.h"
#include "fast_color_bf.h"
#include "image.h"
#define CHRONO

void colorBilateralfilter(  Image3& img, 
                            double sigma_s,
                            double sigma_r);