#include "../TONEMAP1/include/tonemap1.h"
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
  img(1,1) = Vector3(3,3,3); 

  EXPECT_NEAR(3, L_white_(img) , .0001f);
}

TEST(ToneMap1Tests, V_iTest0) {
  Vector3 v(0,0,0);
  Image3 img = Image3(2,2);
  img(0,0) = v;
  img(0,1) = Vector3(1,1,1);
  img(1,0) = Vector3(2,2,2);
  img(1,1) = Vector3(3,3,3); 
  int i = 0;
  int x = 0;
  int y = 0;
  Real s = 1.6;

  EXPECT_NEAR(0.13069, V_i(img, i, x, y, s) , .0001f);
}

TEST(ToneMap1Tests, V_iTest1) {
  auto v = Vector3(3,3,3);
  int width = 9;
  int height = 9;
  Image3 img = Image3(width,height);
  for(int i = 0; i < width; i++) {
    for(int  j = 0; j < height; j++) {
        img(i,j) = v;
    }
  }
  int i = 1;
  int x = 5;
  int y = 5;
  Real s = 3;

  EXPECT_NEAR(Luminance(v), V_i(img, i, x, y, s) , .1f);
}

TEST(ToneMap1Tests, R_iTest0) {
  int x = 0;
  int y = 0;
  Real s = Real(1.0);
//   Real alpha_0 = Real(.35);
  EXPECT_NEAR(2.5984, R_i(0,x,y,s), .0001f);
}

TEST(ToneMap1Tests, VTest0) {
  auto v = Vector3(3,3,3);
  int width = 9;
  int height = 9;
  Image3 img = Image3(width,height);
  for(int i = 0; i < width; i++) {
    for(int  j = 0; j < height; j++) {
        img(i,j) = v;
    }
  }
  Real s = Real(2.56);
  int i = 5;
  int j = 5; 
  EXPECT_NEAR(0 , V(img, s, i, j), .05);
}

TEST(ToneMap1Tests, RightSTest) {
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