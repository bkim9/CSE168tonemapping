#include <cmath>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "load_EXR.h"
#include "linear_bf.h"
#include "image.h"

typedef Image_file::EXR::image_type image_type;
typedef image_type::channel_type    channel_type;

double log_function(const double x);

double exp_function(const double x);

void tonemap2(Image3& img, double sigma_s, double sigma_r, double contrast) ;