#include "hwfinalproject.h"
#include <gtest/gtest.h>
const char* filename = "bath_r3000_1000.exr";
const std::filesystem::path filepath = "bath_r3000_1000.exr";



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
}

TEST(ToneMapTests, Tonemap1Test) {
  auto img = imread3(filepath);

  localtonemap1(img);

  imwrite("givenimglocaltonemap1.exr",img);
}
//  ---------------------1-----------------------
// //  ---------------------2-----------------------
// TEST(ToneMap2Tests, Luminance2Test) {
//   auto img = imread3(filepath);
//   EXPECT_NEAR(25.854531250000001,Luminance2(img(0,0)), .001);
// }

// TEST(ToneMap2Tests, LuminanceLayerTest) {
//   auto img = imread3(filepath);
//   Image1 img1(img.width, img.height);
//   LuminanceLayer(img, img1);
//   EXPECT_NEAR(25.854531250000001,img1(0,0), .001);
// }

// TEST(ToneMap2Tests, logImgTest) {
//   auto img = imread3(filepath);
//   Image1 logI(img.width, img.height);
//   logImg(img, logI);
//   EXPECT_NEAR(log(25.854531250000001)/log(10.0),logI(0,0), .001);
// }

// TEST(ToneMap2Tests, maxITest) {
//   auto img = imread3(filepath);
//   Image1 logI(img.width, img.height);
//   logImg(img, logI);
  
//   EXPECT_NEAR(3.5029662527011629,maxI(logI), .001);
// }

// TEST(ToneMap2Tests, minIFTest) {
//   auto img = imread3(filepath);
//   Image1 logI(img.width, img.height);
//   logImg(img, logI);

//   EXPECT_NEAR(logI(361,122), minI(logI),    .001);
// }

// TEST(ToneMap2Tests, logFTest) {
//   auto img = imread3(filepath);
//   double sigma_s = 16.0;
//   double sigma_r = 0.1;
//   double contrast = 150.0;
//   int w = img.width;
//   int h = img.height;
//   Image1 logI(w,h); 
//   logImg(img,logI);
//   Image1 logF(w,h);
//   // (d) filter log(I) using the bilateral filter to get log(F)
//   bilateralFilter(logI,sigma_s, sigma_r, logF);
  
//   EXPECT_NEAR(minI(logF),logF(365,144), .001);
// }

// TEST(ToneMap2Tests, DTest) {
//   auto img = imread3(filepath);
//   double sigma_s = 16.0;
//   double sigma_r = 0.1;
//   double contrast = 150.0;
//   int w = img.width;
//   int h = img.height;
//   Image1 logI(w,h); 
//   logImg(img,logI);
//   Image1 logF(w,h);
//   // (d) filter log(I) using the bilateral filter to get log(F)
//   bilateralFilter(logI,sigma_s, sigma_r, logF);
  
//   // (e) compute a detail channel D = log(I) - log(F)
//   Image1 D(w,h);
//   Image3 D3(w,h);
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       D(i,j) = logI(i,j) - logF(i,j);
//       D3(i,j).x = D(i,j);
//       D3(i,j).y = D(i,j);
//       D3(i,j).z = D(i,j);
//   }
//   }
//   imwrite("tone2mapD.exr",D3);
//   // EXPECT_NEAR(minI(logF),logF(365,144), .001);
// }

// TEST(ToneMap2Tests, DeltaTest) {
//   auto img = imread3(filepath);
//   double sigma_s = 16.0;
//   double sigma_r = 0.1;
//   double contrast = 150.0;
//   int w = img.width;
//   int h = img.height;
//   Image1 logI(w,h); 
//   logImg(img,logI);
//   Image1 logF(w,h);
//   // (d) filter log(I) using the bilateral filter to get log(F)
//   bilateralFilter(logI,sigma_s, sigma_r, logF);
  
//   // (e) compute a detail channel D = log(I) - log(F)
//   Image1 D(w,h);
//   Image3 D3(w,h);
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       D(i,j) = logI(i,j) - logF(i,j);
//       D3(i,j).x = D(i,j);
//       D3(i,j).y = D(i,j);
//       D3(i,j).z = D(i,j);
//   }
//   }
//   // (f) compute delta = max[log(F)] - min [log(F)]
//   Real delta = maxI(logF) - minI(logF);
//   EXPECT_NEAR(7.4514350927427877,delta, .001);
// }

// TEST(ToneMap2Tests, GammaTest) {
//   auto img = imread3(filepath);
//   double sigma_s = 16.0;
//   double sigma_r = 0.1;
//   double contrast = 150.0;
//   int w = img.width;
//   int h = img.height;
//   Image1 logI(w,h); 
//   logImg(img,logI);
//   Image1 logF(w,h);
//   // (d) filter log(I) using the bilateral filter to get log(F)
//   bilateralFilter(logI,sigma_s, sigma_r, logF);
  
//   // (e) compute a detail channel D = log(I) - log(F)
//   Image1 D(w,h);
//   Image3 D3(w,h);
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       D(i,j) = logI(i,j) - logF(i,j);
//       D3(i,j).x = D(i,j);
//       D3(i,j).y = D(i,j);
//       D3(i,j).z = D(i,j);
//   }
//   }
//   // (f) compute delta = max[log(F)] - min [log(F)]
//   Real delta = maxI(logF) - minI(logF);
//   // std::cout<< "delta: " << delta <<std::endl;

//   // (g) compute gamma = log(contrast) / delta
//   Real gamma = log(contrast) / log(10.) / delta;

//   EXPECT_NEAR( 0.29203653148304975, gamma, .001);
// }

// TEST(ToneMap2Tests, NTest) {
//   auto img = imread3(filepath);
//   double sigma_s = 16.0;
//   double sigma_r = 0.1;
//   double contrast = 150.0;
//   int w = img.width;
//   int h = img.height;
//   Image1 logI(w,h); 
//   logImg(img,logI);
//   Image1 logF(w,h);
//   // (d) filter log(I) using the bilateral filter to get log(F)
//   bilateralFilter(logI,sigma_s, sigma_r, logF);
  
//   // (e) compute a detail channel D = log(I) - log(F)
//   Image1 D(w,h);
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       D(i,j) = logI(i,j) - logF(i,j);
//   }
//   }
//   // (f) compute delta = max[log(F)] - min [log(F)]
//   Real delta = maxI(logF) - minI(logF);

//   // (g) compute gamma = log(contrast) / delta
//   Real gamma = log(contrast) / log(10.) / delta;

//   // (h) compute the new intensity layter N = 10^{gamma * logF + D}
//   Image1 N(w,h);
//   Image3 N3(w,h);
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       N(i,j) = pow(10.,gamma * logF(i,j) + D(i,j));
//       N3(i,j).x = N(i,j);
//       N3(i,j).y = N(i,j);
//       N3(i,j).z = N(i,j);
//   }
//   }
//   imwrite("tone2mapN.exr",N3);
// }

// TEST(ToneMap2Tests, ITest) {
//   auto img = imread3(filepath);
//   double sigma_s = 16.0;
//   double sigma_r = 0.1;
//   double contrast = 50.0;
//   int w = img.width;
//   int h = img.height;
//   Image1 logI(w,h); 
//   logImg(img,logI);
//   Image1 logF(w,h);
//   // (d) filter log(I) using the bilateral filter to get log(F)
//   bilateralFilter(logI,sigma_s, sigma_r, logF);
  
//   // (e) compute a detail channel D = log(I) - log(F)
//   Image1 D(w,h);
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       D(i,j) = logI(i,j) - logF(i,j);
//   }
//   }
//   // (f) compute delta = max[log(F)] - min [log(F)]
//   Real delta = maxI(logF) - minI(logF);

//   // (g) compute gamma = log(contrast) / delta
//   Real gamma = log(contrast) / log(10.) / delta;

//   // (h) compute the new intensity layter N = 10^{gamma * logF + D}
//   Image1 N(w,h);
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       N(i,j) = pow(10.,gamma * logF(i,j) + D(i,j));
//   }
//   }

//   Image1 I(w,h);
//   LuminanceLayer(img,I);
//   Image1 N_I(w,h);
//   // (i) scale the img by N/I
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       N_I(i,j) = N(i,j) / I(i,j);
//   }
//   }
//   // auto mean = aveI(N_I);
//   // (i) scale the img by N/I
//   for( int i =0; i < w; i++) {
//   for( int j =0; j < h; j++) {
//       img(i,j) *= N(i,j) / I(i,j);
//   }
//   }

//   // EXPECT_NEAR(.5, mean, .001);

//   imwrite("tone2mapN_I.exr",img);
// }

TEST(ToneMapTests, Tonemap2Test) {
  auto img = imread3(filepath);
  double sigma_s = .2 *fmin(img.width,img.height);
  double sigma_r = 0.4;
  double contrast = 50.0;
  // colorBilateralfilter(img, sigma_s, sigma_r);
  // bilateralFilter(img, sigma_s, sigma_r, sampling_s, sampling_r);
  tonemap2(img,sigma_s,sigma_r, contrast);
  imwrite("givenimglocaltonemap2.exr",img);
}
//  ---------------------2-----------------------
//  ---------------------3-----------------------
// LogMap tests
TEST(ToneMap3Tests, img2Scene3Test) {
  auto img = imread3(filepath);
  SCENE* log_scene = (SCENE *) malloc(sizeof(SCENE)*(img.width)*(img.height));
  img2scene3(img, log_scene);
  EXPECT_NEAR(26.4375, log_scene->b, .0001);
}

// LogMap tests
TEST(ToneMap3Tests, Scene32ImgTest) {
  auto img = imread3(filepath);
  SCENE* log_scene = (SCENE *) malloc(sizeof(SCENE)*(img.width)*(img.height));
  img2scene3(img, log_scene);
  Image3 NewImg(img.width,img.height);
  scene32img(NewImg,log_scene);
  EXPECT_NEAR(26.4375, NewImg(0,0).z, .0001);
}

TEST(ToneMap1Tests, Tonemap3Test) {
  auto img = imread3(filepath);
  log_tone(img);
  imwrite("givenimglocaltonemap3.exr",img);
}

