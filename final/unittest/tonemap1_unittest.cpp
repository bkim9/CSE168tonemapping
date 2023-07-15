#include "../TONEMAP1/include/tonemap1.h"
#include "hwfinalproject.h"
#include <gtest/gtest.h>

TEST(ToneMap1Tests, LuminanceTestBlack) {
  Vector3 v(0,0,0);
  EXPECT_NEAR(0, Luminance(v), .0001f);
}

TEST(ToneMap1Tests, LuminanceTestWhite) {
  Vector3 v(1,1,1);
  EXPECT_NEAR(1, Luminance(v), .0001f);
}

TEST(ToneMap1Tests, L_white_Test) {
  Vector3 v(0,0,0);
  Image3 img = Image3(2,2);
  img(0,0) = v;
  img(0,1) = Vector3(1,1,1);
  img(1,0) = Vector3(2,2,2);
  img(1,1) = Vector3(4,4,4); 

  EXPECT_NEAR(4, L_white_(img) , .0001f);
}

TEST(ToneMap1Tests, R_iTest0) {
  const char* filename = "ori/bath.txt";
  FILE *infp;
  infp = fopen(filename, "r");

  int width = 0;
  int height= 0;

  fscanf(infp,"%d",&width);
  fscanf(infp,"%d",&height);

  Image3 img(width,height);

  Vector3 v(.244,.322,.328);
  for( int i = 0; i < img.width; i++ ) {
  for (int j = 0; j < img.height;j++ ) {
    img(i,j) = v;
      // fscanf(infp,"%lf",&img(i,j).x);
      // fscanf(infp,"%lf",&img(i,j).y);
      // fscanf(infp,"%lf",&img(i,j).z);
  }    
  }

  int x = width/2;
  int y = height/2;
  Real s = Real(50);
  int i = 0;
  auto wing = ceil(s * pow(1.6,i));
  Real subTotal = Real(0);

  for (int x_o = -wing; x_o < wing; x_o++) {
  for (int y_o = -wing; y_o < wing; y_o++) {
    // if (s==1) {
    //   subTotal += R_i(i, x_o, y_o, s) / 2.6;
    // }
    // else 
    if (0 <= x+x_o && x+x_o < img.width && 0 <= y+y_o && y+y_o < img.height && 2 * wing < img.width && 2 * wing < img.height) {
      subTotal += R_i(i, x_o, y_o, s);
      subTotal /= s==1? 2.6:1; 
    }
  }
  }
  EXPECT_NEAR(1, subTotal, .001f);
}

TEST(ToneMap1Tests, V_iTest0) {
  const char* filename = "ori/bath.txt";
  FILE *infp;
  infp = fopen(filename, "r");

  int width = 0;
  int height= 0;

  fscanf(infp,"%d",&width);
  fscanf(infp,"%d",&height);

  Image3 img(width,height);

  Vector3 v(.244,.322,.328);
  for( int i = 0; i < img.width; i++ ) {
  for (int j = 0; j < img.height;j++ ) {
    fscanf(infp,"%lf",&img(i,j).x);
    fscanf(infp,"%lf",&img(i,j).y);
    fscanf(infp,"%lf",&img(i,j).z);
  }    
  }
  log_tone( img );
  int i = 1;
  int x = width/2;
  int y = height/2;
  Real s = 2;

  EXPECT_NEAR( 3.2714540040939588, V_i(img, i, x, y, s) , .035f);
}

// TEST(ToneMap1Tests, VTest0) {
//   auto v = Vector3(3,3,3);
//   int width = 9;
//   int height = 9;
//   Image3 img = Image3(width,height);
//   for(int i = 0; i < width; i++) {
//     for(int  j = 0; j < height; j++) {
//         img(i,j) = v;
//     }
//   }
//   Real s = Real(2.56);
//   int i = 5;
//   int j = 5; 
//   EXPECT_NEAR(0 , V(img, s, i, j), .05);
// }

TEST(ToneMap1Tests, RightimgTest) {
  auto v = Vector3(3,3,3);
  int width = 9;
  int height = 9;
  Image3 img = Image3(width,height);
  for(int i = 0; i < width; i++) {
    for(int  j = 0; j < height; j++) {
        img(i,j) = Real(i) * v;
    }
  }
  int i = 5;
  int j = 5;
//   Real s = Real(1.0);
  EXPECT_NEAR(2.56, right_s(img, i, j), .0001f);
}

TEST(ToneMap1Tests, imggivenLocalTest) {
  const char* filename = "ori/bath.txt";
  FILE *infp;
  infp = fopen(filename, "r");

  int width = 0;
  int height= 0;

  fscanf(infp,"%d",&width);
  fscanf(infp,"%d",&height);

  Image3 img(width,height);
  for( int i = 0; i < img.width; i++ ) {
  for (int j = 0; j < img.height;j++ ) {
      fscanf(infp,"%lf",&img(i,j).x);
      fscanf(infp,"%lf",&img(i,j).y);
      fscanf(infp,"%lf",&img(i,j).z);
  }    
  }
  fclose(infp);
  
  // localtonemap1(img);

  // ---------------------- 2 -----------------------
  double sigma_s = 16.0;
  double sigma_r = 0.1;
  double sampling_s = 16;
  double sampling_r = .1;
  // colorBilateralfilter(img, sigma_s, sigma_r);
  bilateralFilter(img, sigma_s, sigma_r, sampling_s, sampling_r);



  // log_tone(img);
  imwrite("givenimglocaltonemap1.exr",img);
  EXPECT_NEAR(2.56, img(0, 2).x, .0001f);
}