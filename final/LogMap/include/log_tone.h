#ifndef TONE_MAIN_H
#define TONE_MAIN_H

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/time.h>
#endif

#define false 0
#define true 1

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rgbe.h"
#include "logmapping.h"
#include "color_convert.h"

void clamp_image ();
void open_RGBE_scene(Image3& img);
int log_tone( Image3& img );
void img2scene3(Image3& img, SCENE* scene3);
void scene32img(Image3& img, SCENE* log_scene);
void write_RGBE_image();
void write_ppm_image ();
void gamma_calc ();
void rec_gamma_calc ();
double max_Luminance_rgb ();
void read_commandline (int argc, char **argv);
void printhelp ();
double current_time ();

#endif //TONE_MAIN_H
