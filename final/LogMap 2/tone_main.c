/*
 * tone_main.c  
 * * Syntax:  logmap -i inputfile -o outputfile [options]
 *
 * Great thanks to Erik Reinhard, the implementation of the logmapping
 * tone mapping was started from his framework posted online.
 *
 * Frederic Drago 
 * last change 05/16/2003
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tone_main.h"
#include "rgbe.h"
#include "color_convert.h"
#include "logmapping.h"

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/time.h>
#endif

static float  expo = 0.0;       // Scene exposure
static double world_lum = 0.0;  // Scene exposure
static float  biasParam = 0.0; 	// Bias Param
static float  gammaval = 1.0;  	// Gamma value should use 2.2
static int    ppm = false;      // Save a ppm file
static float  contParam = 0.0; 	// Contrast improvement
static float  km = 0.125;		// Kernel size multiplier
static float  white = 1.0;      // Max lum value of display - White 0.98
static float  black = 0.02;    	// Min lum value of display - Black
int           fast = false;     // Use fast optimization
int           recgamma = false; // Use Rec.709 gamma formula
int           center = false;   // Use a center weighted world adaptation

// Scene parameters and RGBE IO
rgbe_header_info info;
static char infilename[1024];
static char outfilename[1024];   
int width, height;
int centerx = false, centery = false;
int pixels;
static double *scene_IO;
SCENE *scene;

/*
 * Main program
 */
int
main (int argc, char **argv)
{
  double max_lum;
  double min_lum;
  double start_time1, start_time2;

  start_time1 = current_time();
  read_commandline (argc, argv);
  open_RGBE_scene();

  // Default position for Lwa kernel center, not use by default
  if (!centerx)
	centerx = width / 2;
  if (!centery)
	centery = height / 2;
  
  expo = pow(2,expo); //default exposure is 1, 2^0
  
  //fprintf(stderr, "Exposure: %f\n", expo);
  
  // Tone mapping with Logmapping function 
  fprintf(stderr, "Logmapping: \n");
  fprintf(stderr, "------------------------------ \n");
  rgb_Yxy(scene, width, height, &max_lum, &min_lum, &world_lum);
  if (center == true) {
    center_weight(scene, width, height, centerx, centery, km, &world_lum);
  }
  fprintf(stderr, "Maximum Luminance: %f \n", max_lum);
  fprintf(stderr, "Minimum Luminance: %f \n", min_lum);
  fprintf(stderr, "Dynamic Range = %f \n", max_lum / min_lum);
  fprintf(stderr, "------------------------------ \n");

  start_time2 = current_time();
  logmapping (scene, width, height, max_lum, min_lum, world_lum, 
		  biasParam, contParam, expo, white);
  fprintf(stderr, "\nTone mapping execution time = %.3f sec\n",
  (current_time()-start_time2));
  Yxy_rgb(scene, width, height);
  
  if (gammaval != 1.) {
    if (recgamma == true)
	  rec_gamma_calc();
    else
      gamma_calc();
  }
  
  if (ppm == false)
    write_RGBE_image();
  else {
    clamp_image ();
    write_ppm_image();
  }
  fprintf(stderr, "Total execution time = %.3f sec\n", 
      (current_time()-start_time1));
  return 1;
}

void
open_RGBE_scene()
{
  FILE *infp;
  int x,y;
  double exp_mult;
  if (!(infp = fopen(infilename, "rb"))) {
    fprintf (stderr, "Cannot open input file %s\n", infilename);
	exit(1);
  }

  RGBE_ReadHeader(infp, &width, &height, &info);
  
  scene_IO = (double *) malloc(sizeof(double)*width*height*3);
  if (scene_IO == NULL){
  fprintf(stderr, "Could not allocate memory for scene_IO \n");
  exit(1);
  }

  scene = (SCENE *) malloc(sizeof(SCENE)*(width)*(height));
  if (scene == NULL){
  fprintf(stderr, "Could not allocate memory for scene \n");
  exit(1);
  }
  
  RGBE_ReadPixels_RLE(infp, scene_IO, width, height);
  
  // To do, Should be read directly in correct format
  for (x=0, y=0; x<width*height*3; x+=3, y++) {
  scene[y].r = scene_IO[x];
  scene[y].g = scene_IO[x+1];
  scene[y].b = scene_IO[x+2];
  }
  
  if (info.gamma != 1.) {
    printf("Gamma in RGBE image = %f \n", info.gamma);
    for (x=0; x<width*height; x+=1) {
      scene[x].r = (double)pow (scene[x].r, info.gamma);
      scene[x].g = (double)pow (scene[x].g, info.gamma);
      scene[x].b = (double)pow (scene[x].b, info.gamma);
    }
  }
  if (info.exposure != 1.) {
  printf("Exposure in RGBE image = %f \n", info.exposure);
  exp_mult = pow(info.exposure, 2);
  printf("expmult %f \n", exp_mult);
    for (x=0; x<width*height; x+=1) {
      scene[x].r = (double) (scene[x].r*exp_mult);
      scene[x].g = (double) (scene[x].g*exp_mult);
      scene[x].b = (double) (scene[x].b*exp_mult);
    }
  }
  fclose(infp);
}

void
write_RGBE_image()
{
  int x,y;
  FILE *outfp;
  
  for (x=0, y=0; x<width*height*3; x+=3, y++) {
  scene_IO[x]   = scene[y].r;
  scene_IO[x+1] = scene[y].g;
  scene_IO[x+2] = scene[y].b;
  }


  if (!(outfp = fopen(outfilename, "wb")))
    fprintf (stderr, "Cannot open output file %s\n", outfilename);
  RGBE_WriteHeader(outfp, width, height, NULL);
  RGBE_WritePixels_RLE(outfp, scene_IO, width, height);
  fclose(outfp);
}

// Clamp image highest value to display white
void
clamp_image ()
{
  int x;
  for (x=0; x<width*height; x++) {
    scene[x].r = (scene[x].r > 1) ? 1 : scene[x].r;
    scene[x].g = (scene[x].g > 1) ? 1 : scene[x].g;
    scene[x].b = (scene[x].b > 1) ? 1 : scene[x].b;
  }
}

void
write_ppm_image ()
{
  int x;
  FILE *outfp;
  if (!(outfp = fopen(outfilename, "wb")))
    fprintf (stderr, "Cannot open output file %s\n", outfilename);

  fprintf (outfp, "P6\n");
  fprintf (outfp, "%d %d\n%d\n", width, height, 255);

  for (x=0; x<width*height; x++) {
    fprintf (outfp, "%c%c%c",
         (char) (255.*scene[x].r), (char) (255.*scene[x].g), 
   (char) (255.*scene[x].b));
  }
  fprintf (outfp, "\n");
  fclose(outfp);
}

// Gamma calculation
void
gamma_calc ()
{
  int x;
  float fgamma;
  
  fgamma = 1/gammaval;
  
  for (x=0; x<width*height; x++) {
    scene[x].r = (double)pow (scene[x].r, fgamma);
    scene[x].g = (double)pow (scene[x].g, fgamma);
    scene[x].b = (double)pow (scene[x].b, fgamma);
  }
}

// Rec. 709 Gamma calculation
void
rec_gamma_calc ()
{
  int x;
  float fgamma;
  float slope = 4.5;
  float start = 0.018;
  
  fgamma = (0.45/gammaval)*2;
  
  if (gammaval >= 2.1) {
	start = 0.018 / ((gammaval - 2) * 7.5);
	slope = 4.5 * ((gammaval -2) * 7.5);
  }
  else if (gammaval <= 1.9) {
	start = 0.018 * ((2 - gammaval) * 7.5);
	slope = 4.5 / ((2 - gammaval) * 7.5);
  }

  for (x=0; x<width*height; x++) {
	scene[x].r = scene[x].r<=start?
		scene[x].r*slope:1.099*pow(scene[x].r,fgamma)-0.099;
	scene[x].g = scene[x].g<=start?
		scene[x].g*slope:1.099*pow(scene[x].g,fgamma)-0.099;
	scene[x].b = scene[x].b<=start?
		scene[x].b*slope:1.099*pow(scene[x].b,fgamma)-0.099;
  }
}

//  Read command line arguments
void
read_commandline (int argc, char **argv)
{
  int i;

  if (argc < 5) 
    printhelp ();
    for (i = 1; i < argc; i++) {
    if (strcmp (argv[i], "--help") == false || strcmp (argv[i], "-h") == false)
      printhelp ();
    // Input file
    if (strcmp (argv[i], "-i") == false)
      strcpy (infilename, argv[++i]);
    // Output file
    else if (strcmp (argv[i], "-o") == false)
      strcpy (outfilename, argv[++i]);
    // Write output as ppm file (pic default)
    else if (strcmp (argv[i], "-ppm") == false)
      ppm = true;
    // Write output as ppm file (pic default)
    else if (strcmp (argv[i], "-fast") == false)
      fast = true;
    // Use Rec.709 formula for gamma correction
    else if (strcmp (argv[i], "-recgamma") == false||
			strcmp (argv[i], "-rg") == false)
      recgamma = true;
    // Gamma correction (1 default)
    else if (strcmp (argv[i], "-gamma") == false|| 
			strcmp (argv[i], "-g") == false)
      gammaval = (float)atof (argv[++i]);
    // Image exposure multiplier (logmap)
    else if (strcmp (argv[i], "-exp") == false || 
			strcmp (argv[i], "-e") == false)
      expo = atof(argv[++i]);
	// center weighting function
    else if (strcmp (argv[i], "-center") == false)
      center = true;

	// specific coordinate for center weight
	else if (strcmp (argv[i], "-x") == false)
	  centerx = atoi (argv[++i]);
	else if (strcmp (argv[i], "-y") == false)
	  centery = atoi (argv[++i]);
	else if (strcmp (argv[i], "-kernelmult") == false||
			strcmp (argv[i], "-km") == false)
	  km = atof (argv[++i]);
	
    // Bias function
    else if (strcmp (argv[i], "-bias") == false || 
			strcmp (argv[i], "-b") == false)
      biasParam = (float)atof (argv[++i]);
    // Contrast Improvement function (Not in paper)
    else if (strcmp (argv[i], "-contrast") == false ||
			strcmp (argv[i], "-c") == false)
      contParam = (float)atof (argv[++i]);
    // Black value of the screen
    else if (strcmp (argv[i], "-black") == false)
      black = (float)atof (argv[++i]);
    // Min value to map to white
    else if (strcmp (argv[i], "-white") == false)
      white = (float)atof (argv[++i]);
  }
}

// Print the help screen
void
printhelp ()    // print help commands to the console
{
  fprintf (stderr, "\nUSAGE: \n");
  fprintf (stderr, "logmap -i input.pic -o output.pic/.ppm \n");
  fprintf (stderr, "OPTIONS: \n");
  fprintf (stderr, "-i \t\tinput file in Radiance rgbe (.pic or .hdr)\n");
  fprintf (stderr, "-o \t\toutput file in Radiance rgbe (.pic .hdr)\n");
  fprintf (stderr, "-ppm \t\tSave a 24 bit ppm file \n");
  fprintf (stderr, "-fast \t\tUse faster algorithm (def: no)\n");
  fprintf (stderr, "-bias -b \tslope of the bias curve (0.7~0.99), def: 0.85\n");
  fprintf (stderr, "-contrast -c \tContrast improvement (0.7~0.99), def: no\n");
  fprintf (stderr, "-exp -e \tExposure multliplier (-/+), def: 0.0\n");
  fprintf (stderr, "-gamma -g \tGamma correction value, def: 1.0\n");
  fprintf (stderr, "-recgamma -rg \tuse gamma correction from the paper\n");
  fprintf (stderr, "-center \tUse a Gaussian Kernel for Lwa \n");
  fprintf (stderr, "-km \t\tKernel size multiplier (0~1)\n");
  fprintf (stderr, "-x \t\tKernel center abscissa\n");
  fprintf (stderr, "-y \t\tKernel center ordinate \n");
  //fprintf (stderr, "-black \tScreen black value (def: 0.02)\n");
  //fprintf (stderr, "-white \tMinimum value mapped to white (def: 1.0)\n");
  exit (0);
}

// Operation timing routine
double
current_time ()
{
  double current;

  #ifdef WIN32 
  current = (GetCurrentTime() * 0.001);
  #else 
  struct timeval tp;
  gettimeofday (&tp, NULL);
  current = tp.tv_sec + 1e-6 * tp.tv_usec;
  #endif
  return current;
}
