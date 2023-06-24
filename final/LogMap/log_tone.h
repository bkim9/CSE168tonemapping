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

int log_tone( Image3& img );
void open_RGBE_scene();
void write_RGBE_image();
void write_ppm_image ();
void gamma_calc ();
void rec_gamma_calc ();
void clamp_image ();
double max_Luminance_rgb ();
void read_commandline (int argc, char **argv);
void printhelp ();
double current_time ();

#endif //TONE_MAIN_H
