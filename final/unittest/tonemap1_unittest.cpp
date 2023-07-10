#include "tonemap1.h"
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

TEST(ToneMap1Tests, R_iTest) {
  int x = 0;
  int y = 0;
  Real s = Real(1.0);
  Real alpha_0 = Real(.35);
  EXPECT_NEAR(2.5984, R_i(x,y,s, alpha_0), .0001f);
}

TEST(ToneMap1Tests, RightSTest) {
  int x = 0;
  int y = 0;
  Real s = Real(1.0);
  Real alpha_0 = Real(.35);
  EXPECT_NEAR(2.5984, R_i(x,y,s, alpha_0), .0001f);
}