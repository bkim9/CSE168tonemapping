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

#include "bilateral_filter.h"

using namespace std;

void Image1toimage_type (Image1& img, image_type& image) {
    for( int i =0; i < img.width ; i++ ){
    for( int j = 0; j< img.height; j++ ) {
        image(i,j) = img(i,j);
    }
    }    
}

void image_typetoImage1 (image_type& image, Image1& img) {
    for( int i =0; i < img.width ; i++ ){
    for( int j = 0; j< img.height; j++ ) {
        img(i,j) = image(i,j);
    }
    }    
}



void bilateralFilter( Image1& img,
                      double sigma_s,
                      double sigma_r,
                      Image1& imgF) {

  unsigned width = img.width;
  unsigned height= img.height;

  image_type image(width,height);
  Image1toimage_type(img, image);
  
    cout<<"spatial parameters (measured in pixels)"<<endl;
    cout<<"---------------------------------------"<<endl;
    cout<<"sigma_s    : parameter of the bilateral filter (try 16)"<<endl;
    cout<<"sampling_s : sampling used by the approximation (try 16)"<<endl;
    cout<<endl;
    cout<<"range parameters (intensity is scaled to [0.0,1.0])"<<endl;
    cout<<"---------------------------------------------------"<<endl;
    cout<<"sigma_r    : parameter of the bilateral filter (try 0.1)"<<endl;
    cout<<"sampling_r : sampling used by the approximation (try 0.1)"<<endl;
    cout<<endl;

  // ##############################################################

  cout<<"sigma_s    = "<<sigma_s<<"\n";
  cout<<"sigma_r    = "<<sigma_r<<"\n";
  
  // ##############################################################
  
  cout<<"Filter the image... "<<endl;

  image_type filtered_image(width,height);

  FFT::Support_3D::set_fftw_flags(FFTW_ESTIMATE);
 
  Image_filter::linear_BF(image,
                          sigma_s,sigma_r,
                          sigma_s,sigma_r,
                          &filtered_image);
    
  cout<<"Filtering done"<<endl;

  // ##############################################################
  // image_type -> Image1
  image_typetoImage1(filtered_image,imgF);
}