#include "hwfinalproject.h"
#include <gtest/gtest.h>

const char* filename = "ori/bath1000.txt";
TEST(BlackSceneTests, zerodepthtest) {
  Scene scene;
  Vector3 p(0,0,0);
  Vector3 wo(0,1,0);
  Ray ray{p,wo,Real(.001),infinity<Real>()};
  pcg32_state rng = init_pcg32(0,0);
  int depth = 0;
  auto rad = radiance(scene, ray, rng, depth);
  
  EXPECT_EQ(0, rad.x);
  EXPECT_EQ(0, rad.y);
  EXPECT_EQ(0, rad.z);
}

TEST(BlackSceneTests, emptyScenetest) {
  Scene scene;
  Vector3 p(0,0,0);
  Vector3 wo(0,1,0);
  Ray ray{p,wo,Real(.001),infinity<Real>()};
  pcg32_state rng = init_pcg32(0,0);
  int depth = 1;
  // auto rad = radiance(scene, ray, rng, depth);
  
  EXPECT_FALSE(intersect(scene,ray));
}

TEST(BlackSceneTests, SimpleScenetest) {
  std::string filename = "/Users/naryungkim/Documents/GitHub/CSE168/CSE168tonemapping/final/scenes/cbox/cbox_point_light.xml";
  ParsedScene parsed_scene = parse_scene(filename); 
  Scene scene(parsed_scene);
  scene.samples_per_pixel = 5;
  Vector3 p(0,0,0);
  Vector3 wo(0,1,0);
  Ray ray{p,wo,Real(.001),infinity<Real>()};
  pcg32_state rng = init_pcg32(0,0);
  int depth = 1;
  // auto rad = radiance(scene, ray, rng, depth);
  auto rad = radiance(scene, ray, rng, depth);
  EXPECT_NEAR(0.00238306,rad.x, .0001);
  EXPECT_NEAR(0.00099705,rad.y, .0001);
  EXPECT_NEAR(0.00024037,rad.z, .0001);
}

TEST(SmoothTests, AverageTest) {
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
  int x = 1;
  int y = 1;
  auto ave = averageV(img,x,y);
  std::cout << "center: " << img(x,y) << std::endl;
  std::cout << "average: " << ave << std::endl;
  bool pass = true;
  if( fabs(length(ave) - length(img(x,y)-ave)) / length(ave)< .5) pass = true;
  EXPECT_TRUE(pass);
}

// tone1 test
TEST(TonempaTests, Tonemap1Test) {
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

  localtonemap1(img);
  imwrite("bath_tone1.exr", img);
  // EXPECT_TRUE(pass);
}


// tone2 test
TEST(TonemapTests, bilateralFilterTest) {
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
  // localtonemap1(img);
  double sigma_s = 16.0;
  double sigma_r = 0.1;
  double sampling_s = 16;
  double sampling_r = .1;
  // colorBilateralfilter(img, sigma_s, sigma_r);
  bilateralFilter(img, sigma_s, sigma_r, sampling_s, sampling_r); 
  // smooth(img);
  imwrite("bath_tone2.exr", img);
  // EXPECT_TRUE(pass);
}

// tone3 test
TEST(TonemapTests, LogToneTest) {
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

  log_tone(img);
  imwrite("bath_tone3.exr", img);
  // EXPECT_TRUE(pass);
}

// TEST(hemilocTests, hemilocTestZ) {
//   Real r1 = 0.1;
//   Real z = .1;
//   Vector3 v = hemiloc(r1,z);

//   EXPECT_NEAR(.1, v.z, .001f);
// }

// TEST(RandomToSphereTests, randomToSphere) {
//   Real distance_squared = Real(4.0f);
//   Real area = Real(0);
//   pcg32_state rng;
//   Sphere sph;
//   sph.center = Vector3(0,0,0);
//   sph.radius = Real(2);
//   auto v = sph.random_to_sphere(distance_squared, area, rng);
//   EXPECT_NEAR(0, v.x, .001f);
//   EXPECT_NEAR(0, v.y, .001f);
//   EXPECT_NEAR(2, v.z, .001f);
//   EXPECT_NEAR(0, area, .0001f);
// }

// TEST(RandomToSphereTests, AreaTest) {
//   Real distance_squared = Real(4.0f);
//   Real area = Real(0);
//   pcg32_state rng;
//   Sphere sph;
//   sph.center = Vector3(0,0,0);
//   sph.radius = Real(2);
//   auto v = sph.random_to_sphere(distance_squared, area, rng);
//   EXPECT_NEAR(0, area, .0001f);
// }

// TEST(TriangleTests, AreaTest) {
//   auto p0 = Vector3(0,0,0);
//   auto p1 = Vector3(1,0,0);
//   auto p2 = Vector3(0,2,0);
//   auto n0 = Vector3(0,0,1);
//   auto n1 = Vector3(0,0,1);
//   auto n2 = Vector3(0,0,1);
//   TriangleMesh mesh;
//   mesh.positions.push_back(p0);
//   mesh.positions.push_back(p1);
//   mesh.positions.push_back(p2);
//   mesh.normals.push_back(n0);
//   mesh.normals.push_back(n1);
//   mesh.normals.push_back(n2);
//   Vector3i vi(0,1,2);

//   mesh.indices.push_back(vi);
//   Triangle tri{0,&mesh};
//   tri.setup();
//   EXPECT_NEAR(0, tri.crossproduct.x, .0001f);
//   EXPECT_NEAR(0, tri.crossproduct.y, .0001f);
//   EXPECT_NEAR(2, tri.crossproduct.z, .0001f);
// }