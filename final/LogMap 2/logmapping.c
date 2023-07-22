/*
 * logmapping.c  
 * Implementation of the log mapping operator
 *
 * Frederic Drago
 * Cleaned up May 16 2003
*/  
  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tone_main.h"
  
#define LOG05 -0.693147		// log(0.5) 
#define LOG2 0.693147
#define E 2.718281828

double
bias (double b, double x) 
{
  return pow (x, b);		// pow(x, log(bias)/log(0.5)
}

void
logmapping (SCENE * scene, 
		int width, 
		int height, 
		double Lum_max,
	    double Lum_min, 
		float world_lum, 
		float biasParam,
		float contParam,
		float exposure,
		float white)
{
  double Lmax, divider, av_lum, interpol, biasP, contP;
  int x, y, i, j, index;
  int nrows, ncols;
  double L;
  double exp_adapt;
  double Average;
  extern int fast;

  nrows = height;
  ncols = width;

  // Arbitrary Bias Parameter 
  if (!biasParam) 
	biasParam = 0.85;
  
  fprintf (stderr, "Bias parameter: %f \n", biasParam);
  
  exp_adapt = 1;//pow(biasParam,5);
  av_lum = exp(world_lum) / exp_adapt;
  
  fprintf(stderr, "world adaptation: %f \n", av_lum);
  
  biasP = log(biasParam)/LOG05;
  contP = 1/contParam;
  Lmax = Lum_max/av_lum;
  divider = log10(Lmax+1);
  
  // Normal tone mapping of every pixel
  if (!fast) {
    for (x=0; x < nrows; x++)
	  for (y = 0; y < ncols; y++) {
	    index = x * ncols + y;
		
	    // inverse gamma function to enhance contrast
		// Not in paper
		if (contParam) 
	      scene[index].r = pow(scene[index].r, contP); 
		
	    scene[index].r /= av_lum;
		if (exposure != 1.0)
   	      scene[index].r *= exposure;
		
	    interpol = log (2 + bias(biasP, scene[index].r / Lmax) * 8);
	    
		scene[index].r = ((log (scene[index].r+1)/interpol)/divider);
	  }
  }
  
  /*
   * Approximation of log(x+1)
   *x(6+x)/(6+4x) good if x < 1
   *x*(6 + 0.7662x)/(5.9897 + 3.7658x) between 1 and 2
   *http://users.pandora.be/martin.brown/home/consult/logx.htm
   */
  else {
    for (x=0; x<nrows; x+=3)
	  for (y=0; y<ncols; y+=3) {
	    Average = 0.;
	    for (i=0; i<3; i++)
	      for (j=0; j<3; j++) {
		    scene[(x+i)*ncols+y+j].r /= av_lum;
			if (exposure != 1.)
		      scene[(x+i)*ncols+y+j].r*= exposure; 
		    Average += scene[(x+i)*ncols+y+j].r;
		  }
	    Average = Average / 9 - scene[x*ncols+y].r;
	    if (Average > -1 && Average < 1) {
		  interpol = 
			  log(2+pow(scene[(x+1)*ncols+y+1].r/Lmax, biasP)*8);
		  for (i=0; i<3; i++)
		    for (j=0; j<3; j++) {
			  index = (x+i)*ncols+y+j;
              if (scene[index].r < 1) {
                L = scene[index].r*(6+scene[index].r)/(6+4*scene[index].r);
                scene[index].r = (L/interpol) / divider;
              }
              else if ( scene[index].r >= 1 && scene[index].r < 2) {
                L = scene[index].r*(6+0.7662*scene[index].r)/
                    (5.9897+3.7658*scene[index].r);
	            scene[index].r = (L/interpol) / divider;
	          }
	          else
			  scene[index].r = 
			      (log (scene[index].r + 1)/interpol)/divider;
		    }
	    }
	    else {
		  for (i=0; i<3; i++)
		    for (j=0; j<3; j++) {
		      interpol =
			      log(2+pow(scene[(x+i)*ncols+y+j].r/Lmax, biasP)*8);
		      scene[(x+i)*ncols+y+j].r =
			      (log(scene[(x+i)*ncols+y+j].r+1)/interpol)/divider;
			}
		}
	  } //y
  } // else
  
}
