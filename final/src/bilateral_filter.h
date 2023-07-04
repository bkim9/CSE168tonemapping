#pragma once
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#define CHRONO
#include "linear_bf.h"
#include "image.h"

void bilateralFilter( Image3& img,
                      double sigma_s,
                      double sigma_r,
                      double sampling_s,
                      double sampling_r);