#include "tonemapping2.h"
#include <gtest/gtest.h>



TEST(ToneMap2Tests, BasicLuminance2Test) {
  Vector3 v(0,0,0);
  // EXPECT_EQ(0, Luminance2(v));
}

// Real Luminance2(Vector3 v);

TEST(ToneMap2Tests, BasicLuminanceLayerTest) {
  Vector3 v(2,2,2);
  int w = 2;
  int h = 2;
  Image3 img(w,h);
  for ( int i =0; i < w; i++) {
  for( int j = 0; j <h; j++  ) {
    img(i,j) = v;
  }
  }
  Image1 img1(w,h);
  // LuminanceLayer(img,img1);
  // EXPECT_EQ(Luminance2(v), img1(0,0));
}

// void LuminanceLayer(Image3& img, Image1& img1);
TEST(ToneMap2Tests, BasiclogImgTest) {
  Vector3 v(2,2,2);
  int w = 2;
  int h = 2;
  Image3 img(w,h);
  for ( int i =0; i < w; i++) {
  for( int j = 0; j <h; j++  ) {
    img(i,j) = v;
  }
  }
  Image1 img1(w,h);
  // LuminanceLayer(img,img1);
  Image1 logI(w,h);
  // logImg(img, logI);
  int x = 0;
  int y = 0;
  // EXPECT_NEAR(log(img1(x,y))/log(10.), logI(x,y), .001);
}
// void logImg(Image3& img, Image1 logI);
// TEST(ToneMap2Tests, BasicmaxITest) {
//   Vector3 v(2,2,2);
//   int w = 2;
//   int h = 3;
//   Image3 img(w,h);
//   for ( int i =0; i < w; i++) {
//   for( int j = 0; j <h; j++  ) {
//     img(i,j) = Vector3(i,i,i);
//   }
//   }
//   Image1 logI(w,h);
//   logImg(img, logI);
//   int x = 1;
//   int y = 1;
//   EXPECT_EQ(logI(x,y), maxI(logI));
// }
// Real maxI(Image1& img);
// TEST(ToneMap2Tests, BasicminITest) {
//   Vector3 v(2,2,2);
//   int w = 2;
//   int h = 2;
//   Image3 img(w,h);
//   for ( int i =0; i < w; i++) {
//   for( int j = 0; j <h; j++  ) {
//     img(i,j) = Vector3(i,i,i);
//   }
//   }
//   Image1 logI(w,h);
//   // logImg(img, logI);
//   int x = 0;
//   int y = 0;
//   EXPECT_EQ(logI(x,y), minI(logI));
// }
// Real minI(Image1& img);
// TEST(ToneMap2Tests, BasicBilateralFilterTest) {
//   int w = 200;
//   int h = 200;
//   Image3 img(w,h);
//   for ( int i =0; i < w; i++) {
//   for( int j = 0; j <h; j++  ) {
//     img(i,j) = Vector3(i,i,i);
//   }
//   }
//   double space_sigma = 0.02 * min(img.width,img.height);
//   double range_sigma = 0.4;
//   double contrast = 50.;
//   // tonemap2(img, space_sigma, range_sigma, contrast);

//   EXPECT_EQ(1, img(1,0).x);

//   Image1 logI(w,h); 
//   logImg(img,logI);
//   Image1 logF(w,h);
//   image_type image(w,h);
//   Image1toimage_type(logI, image);
//   EXPECT_NEAR(1,image(10,1), .001);

//   image_type filtered_image(w,h);

//   // bilateralFilter(logI,space_sigma, range_sigma, logF);
//   // FFT::Support_3D::set_fftw_flags(FFTW_ESTIMATE);
 
//   // Image_filter::linear_BF(image,
//   //                         space_sigma, range_sigma,
//   //                         space_sigma, range_sigma,
//   //                         &filtered_image);
//   // EXPECT_NEAR(-4,filtered_image(0,0), .001);
//   // EXPECT_NEAR(200,logF(0,0), .001);
//   tonemap2(img, space_sigma, range_sigma, contrast);
//   // imwrite("tonemap2out.exr",img );
//   // Real sigma_r = range_sigma / range_sigma; 
//   // Real sigma_s = space_sigma / space_sigma;
//   // Real padding_xy = (2.0 * sigma_s) + 1; // 3
//   // Real padding_z  = (2.0 * sigma_r) + 1; // 3
//   // EXPECT_EQ(3, padding_xy);



// }
// // void tonemap2(Image3& img, double sigma_s, double sigma_r, double contrast) ;