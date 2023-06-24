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

Image3 hw_4_1(const std::vector<std::string> &params);
Image3 hw_4_2(const std::vector<std::string> &params);
Image3 hw_4_3(const std::vector<std::string> &params);
Image3 hw_s();
