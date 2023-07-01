#pragma once
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include "geom.h"
#include "fast_color_bf.h"
#include "image.h"
#define CHRONO

void colorBilateralfilter(  Image3& img, 
                            double sigma_s,
                            double sigma_r);