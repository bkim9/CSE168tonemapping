#ifndef TONE_MAIN_H
#define TONE_MAIN_H

#define EPSILON 1e-06
#define INF 1E+10

#define false 0
#define true 1

typedef struct {
	double r, g, b;
}SCENE;

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
