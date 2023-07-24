/* 
 * Convert RGB to and from Yxy, same as in Reinhard et al. Sig 2002
 * Added a Gaussian kernel for Lwa
 *
 * Frederic Drago
 * Last modification 05/16/2003
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "color_convert.h"
#include "tone_main.h"

double RGB2Yxy[3][3] = { {0.5141364, 0.3238786, 0.16036376},
                         {0.265068, 0.67023428, 0.06409157},
                         {0.0241188, 0.1228178, 0.84442666}
                        };
double Yxy2RGB[3][3] = { {2.5651, -1.1665, -0.3986},
						 {-1.0217, 1.9777, 0.0439}, 
						 {0.0753, -0.2543, 1.1892} 
						};

// Convert double floating point RGB data to Yxy, return max and min luminance
// and absolute value of luminance log average for automatic exposure. 
void
rgb_Yxy (SCENE *scene, int width, int height, double *maximum, double *minimum, double *log_av)
{
  int x, i;
  double W, result[3];
  double max, min;
  double sum = 0.0;
  int array_size = width * height;

  max = EPSILON;
  min = INF;
  for (x = 0; x < array_size; x++) 
  {
      result[0] = result[1] = result[2] = 0.;
	  for (i = 0; i < 3; i++){
          result[i] += RGB2Yxy[i][0] * scene[x].r;
          result[i] += RGB2Yxy[i][1] * scene[x].g;
          result[i] += RGB2Yxy[i][2] * scene[x].b;
	  }
		  
      if ((W = result[0] + result[1] + result[2]) > 0.) { 
		scene[x].r = result[1];     // Y 
        scene[x].g = result[0] / W;	// x 
        scene[x].b = result[1] / W;	// y 	
      } 
	  else
        scene[x].r = scene[x].g = scene[x].b = 0.;

      max = (max < scene[x].r) ? scene[x].r : max;	// Max Luminance in Scene
      min = (min > scene[x].r) ? scene[x].r : min;	// Min Luminance in Scene
      sum += log(2.3e-5 + scene[x].r); //Contrast constant Tumblin paper
  }
  *maximum = max;
  *minimum = min;
  *log_av = (sum / (width * height)); // Average log lum

  //fprintf (stderr,"Log average = %f \n", *log_av);
 }

// Convert Yxy image back to double floating point RGB
void
Yxy_rgb (SCENE *scene, int width, int height)
{
  int x, i;
  double result[3];
  double X, Y, Z;
  int array_size = width * height;

  for (x = 0; x < array_size; x++ ) {
      Y = scene[x].r;	        // Y 
      result[1] = scene[x].g;	// x 
      result[2] = scene[x].b;	// y 
      if ((Y > EPSILON) && (result[1] > EPSILON) && (result[2] > EPSILON)) {
        X = (result[1] * Y) / result[2];
        Z = (X / result[1]) - X - Y;
      } else
        X = Z = EPSILON;
      scene[x].r = X;
      scene[x].g = Y;
      scene[x].b = Z;
      result[0] = result[1] = result[2] = 0.;
      for (i = 0; i < 3; i++){
          result[i] += Yxy2RGB[i][0] * scene[x].r;
          result[i] += Yxy2RGB[i][1] * scene[x].g;
          result[i] += Yxy2RGB[i][2] * scene[x].b;
	  }
      scene[x].r = result[0];
      scene[x].g = result[1];
      scene[x].b = result[2];
    }
}

// A center weighted world adaptation based on a gaussian kernel
void
center_weight (SCENE *scene, 
		int width, 
		int height, 
		int centerx, 
		int centery,
		float km,
		double *log_av)
{
  int x, y,i,j,index;
  float r,rclip,pixnb;
  int khalf;
  int kernel_size;
  float **mask;
  int xstart,ystart;
  double sum = 0.0, m = 0.0;
  pixnb = 0.;
  
  kernel_size = width<height?width*km:height*km;

  // make sure the kernel size is an odd number
  if (kernel_size % 2 == 0)
	kernel_size -= 1;
  
  fprintf(stderr,"Kernel size: %d pixels.\n", kernel_size);
  
  khalf = floor( kernel_size * 0.5);
  
  // If the kernel has some part outside the image, the kernel is moved  to the 
  // closest possible coordinate
  if (centerx + khalf > width)
	xstart = width - kernel_size;
  else if (centerx - khalf < 0)
	xstart = 0; 
  else 
    xstart = centerx-khalf;
  
  if (centery + khalf > height)
	ystart = height - kernel_size;
  else if (centery - khalf < 0)
	ystart = 0;
  else 
    ystart = centery-khalf;

  fprintf(stderr,"Kernel center (topleft = 0,0),x: %d y: %d \n\n",
		  (int)(xstart+kernel_size*0.5), (int)(ystart+kernel_size*0.5));
  
  // memory allocation for the kernel
  mask = (float **) malloc(sizeof(float*)* kernel_size);
  for (i=0;i<kernel_size;i++)
	mask[i] = (float *) malloc(sizeof(float)* kernel_size);
  if (mask == NULL){
  fprintf(stderr, "Could not allocate memory for Gaussian kernel \n");
  exit(1);
  }

/*
 * Build the Gaussian kernel
 */
  rclip =  (double)khalf*sqrt(-log(0.1)/log(2.0));
  for (x = -khalf; x <= khalf; x++) {
    for (y = -khalf; y <= khalf; y++) {
      r = sqrt(x * x + y * y);
      mask[x+khalf][y+khalf] = exp(-log(2)*pow((r/khalf),2) );
      pixnb += mask[x+khalf][y+khalf];
    }
  }
  m = (kernel_size*kernel_size)/pixnb;  
  for (x=xstart,i=0; x < xstart+kernel_size; x++,i++)
    for (y=ystart,j=0; y < ystart+kernel_size; y++,j++) {
      index = x * (ystart+kernel_size) + y;

      sum += log(2.3e-5+(scene[index].r*mask[i][j]*m));
  }
  *log_av = (sum / (kernel_size*kernel_size)); // Average log lum

  //fprintf (stderr,"Log average = %f \n", *log_av);
  
 }
