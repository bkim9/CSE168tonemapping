#include "tonemapping2.h"


double log_function(const double x){
  static const double inv_log_base = 1.0 / log(10.0);
  static const double small_number = 10E-10;
  return log(x+small_number) * inv_log_base;
}


double exp_function(const double x){
  return pow(10.0,x);
}




void tonemap2(Image3& img, double sigma_s, double sigma_r, double contrast) {
    int w = img.width;
    int h = img.height;

    int index =0;
    image_type input_RGBA(w,h);
    // input_RGBA->resize(w,h);
    // img -> input_RGBA
    for(channel_type::iterator
        r     = input_RGBA[image_type::RED].begin(),
        g     = input_RGBA[image_type::GREEN].begin(),
        b     = input_RGBA[image_type::BLUE].begin();
        index != w*h;
        index++,r++,g++,b++){     
        *r = img(index).x;
        *g = img(index).y;
        *b = img(index).z;
    }

  // ##############################################################

  
  std::cout<<"Compute the log-intensity channel... "<<std::flush;

  channel_type intensity_channel(w,h);
  channel_type log_intensity_channel(w,h);

  for(channel_type::iterator
	i     = intensity_channel.begin(),
	i_end = intensity_channel.end(),
	r     = input_RGBA[image_type::RED].begin(),
	g     = input_RGBA[image_type::GREEN].begin(),
	b     = input_RGBA[image_type::BLUE].begin(),
	l     = log_intensity_channel.begin();
      i != i_end;
      i++,r++,g++,b++,l++){

    // The input value are not gamma-corrected. We do not need to worry about that.
       
    *i = ((20.0*(*r) + 40.0*(*g) + 1.0*(*b)) / 61.0);
    *l = log_function(*i);
  }

  std::cout<<"Done"<<std::endl;


  // ##############################################################
  
  
//   std::cout<<"Filter the log-intensity channel... "<<flush;

  channel_type filtered_log_intensity_channel(w,h);

  FFT::Support_3D::set_fftw_flags(FFTW_ESTIMATE); // parameter for FFTW

  const double space_sigma = 0.02 * fmin(w,h);
  const double range_sigma = 0.4;
  
  Image_filter::linear_BF(log_intensity_channel,
			  space_sigma,
			  range_sigma,
			  &filtered_log_intensity_channel);
  
  std::cout<<"Done"<<std::endl;

  const double max_i = *max_element(filtered_log_intensity_channel.begin(),
					filtered_log_intensity_channel.end());
  
  const double min_i = *min_element(filtered_log_intensity_channel.begin(),
					filtered_log_intensity_channel.end());

  std::cout << "max_f: " << max_i << std::endl;
  std::cout << "min_f: " << min_i << std::endl;  

  // ##############################################################
  
  
  std::cout<<"Compute the detail channel... "<<std::flush;

  channel_type detail_channel(w,h);

  for(channel_type::iterator
	l     = log_intensity_channel.begin(),
	l_end = log_intensity_channel.end(),
	f     = filtered_log_intensity_channel.begin(),
	d     = detail_channel.begin();
      l != l_end;
      l++,f++,d++){

    *d = (*l) - (*f);
  }

  std::cout<<"Done"<<std::endl;


  // ##############################################################

  
  std::cout<<"Compute the new intensity channel... "<<std::flush;

  const double max_value = *max_element(filtered_log_intensity_channel.begin(),
					filtered_log_intensity_channel.end());
  
  const double min_value = *min_element(filtered_log_intensity_channel.begin(),
					filtered_log_intensity_channel.end());

  const double gamma = log_function(contrast) /  (max_value - min_value);

  // std::cout << "orimaxF: " << max_value << std::endl;
  // std::cout << "oriminF: " << min_value << std::endl;
  // std::cout << "oridelta: " << (max_value - min_value) << std::endl;
  channel_type new_intensity_channel(w,h);

  for(channel_type::iterator
	f     = filtered_log_intensity_channel.begin(),
	f_end = filtered_log_intensity_channel.end(),
	d     = detail_channel.begin(),
	n     = new_intensity_channel.begin();      
      f != f_end;
      f++,d++,n++){

    *n = exp_function((*f) * gamma + (*d));
  }
  
  std::cout<<"Done"<<std::endl;

  
  // ##############################################################

  
  std::cout<<"Recompose the color image... "<<std::flush;

  image_type output_RGBA = input_RGBA;

  for(channel_type::iterator
	n     = new_intensity_channel.begin(),
	n_end = new_intensity_channel.end(),
	i     = intensity_channel.begin(),
	r     = output_RGBA[image_type::RED].begin(),
	g     = output_RGBA[image_type::GREEN].begin(),
	b     = output_RGBA[image_type::BLUE].begin();
      n != n_end;
      n++,i++,r++,g++,b++){

    const double ratio = (*n) / (*i);
    
    *r *= ratio;
    *g *= ratio;
    *b *= ratio;
    // std::cout<< "r: " << *r << std::endl << std::endl;

  }

  std::cout<<"Done"<<std::endl;
  

  // ##############################################################

  // Scale factor to ensure that the base spans [0;1].
  const double scale_factor = 1.0 / exp_function(max_value * gamma);

  
//   for(unsigned y=0;y<h;y++){

//     const unsigned ry = height - y - 1; // We flip the vertical axis.
    
//     for(unsigned x=0;x<w;x++){

//       // The following lines corresponds to values *without* gamma-correction.
// //       const char r = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*scale_factor*output_RGBA[image_type::RED](x,ry)));
// //       const char g = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*scale_factor*output_RGBA[image_type::GREEN](x,ry)));
// //       const char b = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*scale_factor*output_RGBA[image_type::BLUE](x,ry)));
      
//       // The following lines corresponds to values *with* gamma-correction.
//       const char r = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*));
//       const char g = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*pow(scale_factor*output_RGBA[image_type::GREEN](x,ry),1.0/2.2)));
//       const char b = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*pow(scale_factor*output_RGBA[image_type::BLUE](x,ry),1.0/2.2)));
//     }
//   }

  index=0;
      // img <- output_RGBA
//   for(channel_type::iterator
//     r = output_RGBA[image_type::RED].begin(),
//     g = output_RGBA[image_type::GREEN].begin(),
//     b = output_RGBA[image_type::BLUE].begin()
//     ;index != w*h;
//     index++,r++,g++,b++) {     
//         img(index).x = scale_factor*(*r);
//         img(index).y = scale_factor*(*g);
//         img(index).z = scale_factor*(*b);
//   }
  for(channel_type::iterator
	d     = detail_channel.begin();
    index != w*h;
    index++,d++){
    img(index).x = scale_factor*(*d);
    img(index).y = scale_factor*(*d);
    img(index).z = scale_factor*(*d);
    // *d = (*l) - (*f);
  }

}