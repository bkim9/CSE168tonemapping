#include "../TONEMAP1/include/tonemap1.h"
#include "hwfinalproject.h"
#include <gtest/gtest.h>
const char* filename = "bath_r300_2000.exr";
const std::filesystem::path filepath = "bath_r300_2000.exr";



TEST(ToneMap1LumTests, LuminanceTestBlack) {
  Vector3 v(0,0,0);
  EXPECT_NEAR(0, Luminance(v), .0001f);
}

TEST(ToneMap1LumTests, LuminanceTestWhite) {
  Vector3 v(1,1,1);
  EXPECT_NEAR(1, Luminance(v), .0001f);
}

TEST(ToneMap1LumTests, L_white_Test) {
  Vector3 v(0,0,0);
  Image3 img = Image3(2,2);
  img(0,0) = v;
  img(0,1) = Vector3(1,1,1);
  img(1,0) = Vector3(2,2,2);
  img(1,1) = Vector3(4,4,4); 

  EXPECT_NEAR(4, L_white_(img) , .0001f);
}

TEST(ToneMap1Tests, ImportTest) {
  Image3 img = imread3(filepath);
  EXPECT_TRUE(img.width!=0);
}

//  [1]  smallest
TEST(ToneMap1Tests, R_iTest0) {
  
  Image3 img = imread3(filepath);
  int x = img.width/2;
  int y = img.height/2;
  Real s = Real(100);
  int i = 1;
  auto wing = floor(s * pow(1.6,i-1)/2);
  Real subTotal = Real(0);

  for (int x_o = -wing; x_o <= wing; x_o++) {
  for (int y_o = -wing; y_o <= wing; y_o++) {
    if (0 <= x+x_o && x+x_o < img.width && 0 <= y+y_o && y+y_o < img.height && 2 * wing < img.width && 2 * wing < img.height) {
      subTotal += R_i(i, x_o, y_o, s);
    }
  }
  }
  EXPECT_NEAR(.9191, subTotal, .01f);
}

TEST(ToneMap1Tests, VTest0) {
  Image3 img = imread3(filepath);
  Real s = Real(1);
  int i = 5;
  int j = 5; 
  EXPECT_NEAR(0 , V(img, s, i, j), .05);
}

TEST(ToneMap1Tests, RightSTest) {
  Image3 img = imread3(filepath);
  int i = 5;
  int j = 5;
  EXPECT_NEAR(1, right_s(img, i, j), .0001f);
}

TEST(ToneMap1GlobalTests, LogAverageLTest) {
  int width = 3;
  int height = 3;
  Real e = 2.7182818284;
  Image3 img(width,height); //imread3(filepath);
  for( int i =0; i < width; i++ ) {
  for( int j = 0; j < height; j++) {
    img(i,j) = Vector3(e,e,e); 
    EXPECT_NEAR(e, Luminance(img(i,j)), .001);
  }
  }

  auto la = logAverageL(img);
  EXPECT_NEAR( pow(e,9)/9.0, la, .1);
}

TEST(ToneMap1GlobalTests, scaleLTest) {
  int width = 3;
  int height = 3;
  Real e = 2.7182818284;
  Image3 img(width,height); //imread3(filepath);
  for( int i =0; i < width; i++ ) {
  for( int j = 0; j < height; j++) {
    img(i,j) = Vector3(e,e,e); 
  }
  }
  globaltonemap1(img);
  for( int i =0; i < width; i++ ) {
  for( int j = 0; j < height; j++) {
    EXPECT_NEAR(1, Luminance(img(i, j)), .01f);
  }
  }
  imwrite("tonemap1out.exr", img);
}

TEST(ToneMap1GlobalTests, realEXRlogAverageTest) {
  auto img = imread3(filepath);
  EXPECT_TRUE(0 == logAverageL(img) || infinity<Real>() == logAverageL(img));
  // imwrite("tonemap1out.exr", img);
  // localtonemap1(img);

  // // ---------------------- 2 -----------------------
  // double sigma_s = 16.0;
  // double sigma_r = 0.1;
  // double sampling_s = 16;
  // double sampling_r = .1;
  // // colorBilateralfilter(img, sigma_s, sigma_r);
  // // bilateralFilter(img, sigma_s, sigma_r, sampling_s, sampling_r);

  // // log_tone(img);
  // imwrite("givenimglocaltonemap1.exr",img);
  // EXPECT_NEAR(2.56, img(0, 2).x, .0001f);
}

TEST(ToneMap1LocalTests, Tonemap1Test) {
  auto img = imread3(filepath);

  localtonemap1(img);

  // // ---------------------- 2 -----------------------
  // double sigma_s = 16.0;
  // double sigma_r = 0.1;
  // double sampling_s = 16;
  // double sampling_r = .1;
  // // colorBilateralfilter(img, sigma_s, sigma_r);
  // // bilateralFilter(img, sigma_s, sigma_r, sampling_s, sampling_r);

  // // log_tone(img);
  imwrite("givenimglocaltonemap1.exr",img);
  // EXPECT_NEAR(2.56, img(0, 2).x, .0001f);
}

TEST(ToneMap1LocalTests, Tonemap2Test) {
  auto img = imread3(filepath);
  // // ---------------------- 2 -----------------------
  double sigma_s = 16.0;
  double sigma_r = 0.1;
  double sampling_s = 16;
  double sampling_r = .1;
  // colorBilateralfilter(img, sigma_s, sigma_r);
  bilateralFilter(img, sigma_s, sigma_r, sampling_s, sampling_r);

  imwrite("givenimglocaltonemap2.exr",img);
}

TEST(ToneMap1LocalTests, Tonemap3Test) {
  auto img = imread3(filepath);
  log_tone(img);
  imwrite("givenimglocaltonemap3.exr",img);
}

