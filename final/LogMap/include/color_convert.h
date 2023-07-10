#ifndef COLOR_CONVERT_H
#define COLOR_CONVERT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "log_SCENE.h"

void rgb_Yxy (SCENE *, int width, int height, double *maximum, double *minimum, double *log_av);

void Yxy_rgb (SCENE *, int width, int height);

void center_weight (SCENE *scene, int width, int height, int centerx, int centery, float km, double *log_av);

#endif // COLOR_CONVERT_H

