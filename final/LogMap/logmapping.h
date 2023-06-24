#ifndef LOGMAPPING_H
#define LOGMAPPING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "log_SCENE.h"

#define LOG05 -0.693147		// log(0.5) 
#define LOG2 0.693147
#define E 2.718281828

void logmapping (SCENE *scene, int width, int height, double Lum_max,
		double Lum_min, float  expo, 
		float biasParam, float contParam, float exposure, float white);

#endif //LOGMAPPING_

