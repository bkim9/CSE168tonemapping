#pragma once
#include "scene.h"
#include "bbox.h"
#include "camera.h"
#include "material.h"
#include "parallel.h"
#include "parse_scene.h"
#include "pcg.h"
#include "print_scene.h"
#include "progressreporter.h"
#include "ray.h"
#include "scene.h"
#include "timer.h"
#include "pdf.h"
#include <optional>
#include "image.h"
#include "../TONEMAP1/include/tonemap1.h"
#include "color_bilateral_filter.h"
#include "bilateral_filter.h"
#include "log_tone.h"
#include <fstream>
#include <iostream>
void exportimg(Image3 img, std::string filename);
Vector3 radiance(const Scene &scene, Ray ray, pcg32_state rng, int depth);
Image3 hw_fin_1(const std::vector<std::string> &params);
Image3 hw_fin_2(const std::vector<std::string> &params);
Image3 hw_fin_3(const std::vector<std::string> &params);
