/*! \file
  \verbatim
  
    Copyright (c) 2006, Sylvain Paris and Fr�do Durand

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  \endverbatim
*/

#include "color_bilateral_filter.h"

#define CHRONO


using namespace std; 

typedef Array_2D<Geometry::Vec3<double> > image_type;


void colorBilateralfilter(  Image3& img, 
                            double sigma_s,
                            double sigma_r){

  unsigned width = img.width;
  unsigned height= img.height;

  image_type image(width,height);
  
  // Image3 -> image_type
  for(unsigned y=0;y<height;y++){
    for(unsigned x=0;x<width;x++){
      image(x,y)[0] = static_cast<double>(img(x,y).x);
      image(x,y)[1] = static_cast<double>(img(x,y).y);
      image(x,y)[2] = static_cast<double>(img(x,y).z);
    }
  }
  

  cout<<"image conversion Done"<<endl;

  cout<<"sigma_s    = "<<sigma_s<<"\n";
  cout<<"sigma_r    = "<<sigma_r<<"\n";

  // ##############################################################
  
  
  cout<<"Filter the image... "<<endl;

  image_type filtered_image(width,height);
 
  Image_filter::fast_color_BF(image,
			      sigma_s,sigma_r,
			      &filtered_image);
  
  cout<<"Filtering done"<<endl;

  // ##############################################################
  
  
  cout<<"output -> Image3 '";


  
  // image_type -> Image3
  for(int y=0;y<height;y++){    
    for(int x=0;x<width;x++){
      double R = filtered_image(x,y)[0];
      double G = filtered_image(x,y)[1];
      double B = filtered_image(x,y)[2];
      
      img(x,y).x = std::clamp(R,0.0,1.0);
      img(x,y).y = std::clamp(G,0.0,1.0);
      img(x,y).z = std::clamp(B,0.0,1.0);
    }
  }
  cout<<"Done"<<endl;
}
