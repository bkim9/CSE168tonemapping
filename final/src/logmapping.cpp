/*
 * logmapping.c  
 * Implementation of the log mapping operator
 *
 * Frederic Drago
 * Cleaned up May 16 2003
*/  
#include "logmapping.h"

double
bias (double b, double x) {
  return pow (x, b);		// pow(x, log(bias)/log(0.5)
}


// out log_scene:: SCENE*
void logmapping (SCENE * log_scene, 
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
	      log_scene[index].r = pow(log_scene[index].r, contP); 
		
	    log_scene[index].r /= av_lum;
		if (exposure != 1.0)
   	      log_scene[index].r *= exposure;
		
	    interpol = log (2 + bias(biasP, log_scene[index].r / Lmax) * 8);
	    
		log_scene[index].r = ((log (log_scene[index].r+1)/interpol)/divider);
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
		    log_scene[(x+i)*ncols+y+j].r /= av_lum;
			if (exposure != 1.)
		      log_scene[(x+i)*ncols+y+j].r*= exposure; 
		    Average += log_scene[(x+i)*ncols+y+j].r;
		  }
	    Average = Average / 9 - log_scene[x*ncols+y].r;
	    if (Average > -1 && Average < 1) {
		  interpol = 
			  log(2+pow(log_scene[(x+1)*ncols+y+1].r/Lmax, biasP)*8);
		  for (i=0; i<3; i++)
		    for (j=0; j<3; j++) {
			  index = (x+i)*ncols+y+j;
              if (log_scene[index].r < 1) {
                L = log_scene[index].r*(6+log_scene[index].r)/(6+4*log_scene[index].r);
                log_scene[index].r = (L/interpol) / divider;
              }
              else if ( log_scene[index].r >= 1 && log_scene[index].r < 2) {
                L = log_scene[index].r*(6+0.7662*log_scene[index].r)/
                    (5.9897+3.7658*log_scene[index].r);
	            log_scene[index].r = (L/interpol) / divider;
	          }
	          else
			  log_scene[index].r = 
			      (log (log_scene[index].r + 1)/interpol)/divider;
		    }
	    }
	    else {
		  for (i=0; i<3; i++)
		    for (j=0; j<3; j++) {
		      interpol =
			      log(2+pow(log_scene[(x+i)*ncols+y+j].r/Lmax, biasP)*8);
		      log_scene[(x+i)*ncols+y+j].r =
			      (log(log_scene[(x+i)*ncols+y+j].r+1)/interpol)/divider;
			}
		}
	  } //y
  } // else
  
}
