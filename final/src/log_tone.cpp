/*
 * tone_main.c  
 * * Syntax:  logmap -i inputfile -o outputfile [options]
 *
 * Great thanks to Erik Reinhard, the implementation of the logmapping
 * tone mapping was started from his framework posted online.
 *
 * Frederic Drago 
 * modified by Beomsu Kim in 2023
 * last change 05/16/2003
 */

#include "log_tone.h"


static float  expo = 1.0;       // Scene exposure
static double world_lum = 0.0;  // Scene exposure
static float  biasParam = 0.0; 	// Bias Param
static float  gammaval = 2.2;  	// Gamma value should use 2.2
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
int width, height;
int centerx = false, centery = false;
int pixels;
static double *log_scene_IO;
SCENE *log_scene;

int log_tone( Image3& img ) {
  info.gamma = 2.2;
  info.exposure = 1.0;
  width = img.width;
  height = img.height;
  double max_lum;
  double min_lum;
  double start_time1, start_time2;

  start_time1 = current_time();
  log_scene = (SCENE *) malloc(sizeof(SCENE)*(width)*(height));
  img2log_scene(img, log_scene);

  std::cout << "1. log_scene[0].r: " << log_scene[0].r << std::endl;
  // Default position for Lwa kernel center, not use by default
  if (!centerx)	centerx = width / 2;
  if (!centery)	centery = height / 2;
  // std::cout << "log_scene[0].r: " << log_scene[0].r << std::endl;

  expo = pow(2,expo); //default exposure is 1, 2^0
  std::cout << "2. log_scene[0].r: " << log_scene[0].r << std::endl;
  // Tone mapping with Logmapping function 
  fprintf(stderr, "Logmapping: \n");
  fprintf(stderr, "------------------------------ \n");
  std::cout << "3. log_scene[0].r: " << log_scene[0].r << std::endl;
  rgb_Yxy(log_scene, width, height, &max_lum, &min_lum, &world_lum);
  if (center == true) {
    center_weight(log_scene, width, height, centerx, centery, km, &world_lum);
  }
  fprintf(stderr, "Maximum Luminance: %f \n", max_lum);
  fprintf(stderr, "Minimum Luminance: %f \n", min_lum);
  fprintf(stderr, "Dynamic Range = %f \n", max_lum / min_lum);
  fprintf(stderr, "------------------------------ \n");

  start_time2 = current_time();
  logmapping (log_scene, width, height, max_lum, min_lum, world_lum, 
		  biasParam, contParam, expo, white);
  fprintf(stderr, "\nTone mapping execution time = %.3f sec\n",
  (current_time()-start_time2));
  Yxy_rgb(log_scene, width, height);
  
  if (gammaval != 1.) {
    if (recgamma == true)
	  rec_gamma_calc();
    else
      gamma_calc();
  }
  
  clamp_image ();
  log_scene2img(img,log_scene);
  fprintf(stderr, "Total execution time = %.3f sec\n", 
      (current_time()-start_time1));
  return 1;
}

void
img2log_scene(Image3& img, SCENE* log_scene) {
  int x,y;
  double exp_mult;
  for (x=0; x<width*height; x+=1) {
    int j = x / width;
    int i = x % width;
    log_scene[x].r = img(i,j).x;
    log_scene[x].g = img(i,j).y;
    log_scene[x].b = img(i,j).z;
  }
  std::cout << "101. log_scene[4].r: " << log_scene[4].r << std::endl;
  
  if (log_scene == NULL){
    fprintf(stderr, "Could not allocate memory for log_scene \n");
    exit(1);
  }
  
  if (info.gamma != 1.) {
    printf("Gamma in RGBE image = %f \n", info.gamma);
    for (x=0; x<width*height; x+=1) {
      log_scene[x].r = (double)pow (log_scene[x].r, info.gamma);
      log_scene[x].g = (double)pow (log_scene[x].g, info.gamma);
      log_scene[x].b = (double)pow (log_scene[x].b, info.gamma);
    }
  }
  std::cout << "116. log_scene[4].r: " << log_scene[4].r << std::endl;

  if (info.exposure != 1.) {
  printf("Exposure in RGBE image = %f \n", info.exposure);
  exp_mult = pow(info.exposure, 2);
  printf("expmult %f \n", exp_mult);
    for (x=0; x<width*height; x+=1) {
      log_scene[x].r = (double) (log_scene[x].r*exp_mult);
      log_scene[x].g = (double) (log_scene[x].g*exp_mult);
      log_scene[x].b = (double) (log_scene[x].b*exp_mult);
    }
  }
  std::cout << "127. log_scene[4].r: " << log_scene[4].r << std::endl;
}

void
log_scene2img(Image3& img, SCENE* log_scene) {
  int x;
  for (x=0; x<width*height; x+=1) {
    int j = x / width;
    int i = x % width;
    img(i,j).x = log_scene[x].r;
    img(i,j).y = log_scene[x].g;
    img(i,j).z = log_scene[x].b;
  }
}

// Gamma calculation
void
gamma_calc () {
  int x;
  float fgamma;
  
  fgamma = 1/gammaval;
  
  for (x=0; x<width*height; x++) {
    log_scene[x].r = (double)pow (log_scene[x].r, fgamma);
    log_scene[x].g = (double)pow (log_scene[x].g, fgamma);
    log_scene[x].b = (double)pow (log_scene[x].b, fgamma);
  }
}

// Rec. 709 Gamma calculation
void
rec_gamma_calc () {
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
	log_scene[x].r = log_scene[x].r<=start?
		log_scene[x].r*slope:1.099*pow(log_scene[x].r,fgamma)-0.099;
	log_scene[x].g = log_scene[x].g<=start?
		log_scene[x].g*slope:1.099*pow(log_scene[x].g,fgamma)-0.099;
	log_scene[x].b = log_scene[x].b<=start?
		log_scene[x].b*slope:1.099*pow(log_scene[x].b,fgamma)-0.099;
  }
}

//  Read command line arguments
void
read_commandline (int argc, char **argv) {
  int i;

  if (argc < 5) 
    printhelp ();
    for (i = 1; i < argc; i++) {
    if (strcmp (argv[i], "--help") == false || strcmp (argv[i], "-h") == false)
      printhelp ();
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
printhelp () {   // print help commands to the console 
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

// Clamp image highest value to display white
void
clamp_image () {
  int x;
  for (x=0; x<width*height; x++) {
    log_scene[x].r = (log_scene[x].r > 1) ? 1 : log_scene[x].r;
    log_scene[x].g = (log_scene[x].g > 1) ? 1 : log_scene[x].g;
    log_scene[x].b = (log_scene[x].b > 1) ? 1 : log_scene[x].b;
  }
}

// Operation timing routine
double
current_time () {
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
