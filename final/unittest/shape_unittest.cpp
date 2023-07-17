#include "shape.h"
#include <gtest/gtest.h>

TEST(hemilocTests, hemilocTestRadius) {
  Real r1 = 0.1;
  Real z = .1;
  Vector3 v = hemiloc(r1,z);
  
  EXPECT_EQ(1, length_squared(v));
}

TEST(hemilocTests, hemilocTest0) {
  Real r1 = 0;
  Real z = 1;
  Vector3 v = hemiloc(r1,z);
  
  EXPECT_EQ(0, v.x);
  EXPECT_EQ(0, v.y);
  EXPECT_EQ(1, v.z);
}

TEST(hemilocTests, hemilocTest1) {
  Real r1 = 1;
  Real z = 0;
  Vector3 v = hemiloc(r1,z);
  
  EXPECT_NEAR(1, v.x, .0001);
  EXPECT_NEAR(0, v.y, .0001);
  EXPECT_NEAR(0, v.z, .0001);
}

TEST(hemilocTests, hemilocTest2) {
  Real r1 = 1. / 6.;
  Real z = 0;
  Vector3 v = hemiloc(r1,z);
  
  EXPECT_NEAR(.5, v.x, .0001);
  EXPECT_NEAR(sqrt(3)/2., v.y, .0001);
  EXPECT_NEAR(0, v.z, .0001);
}

TEST(hemilocTests, hemilocTestXY) {
  Real r1 = 0;
  Real z = .1;
  Real sine_theta_squared = 1- z*z;
  Vector3 v = hemiloc(r1,z);
  EXPECT_NEAR(sine_theta_squared, v.x*v.x+v.y*v.y, .001);
}

TEST(hemilocTests, hemilocTestZ) {
  Real r1 = 0.1;
  Real z = .1;
  Vector3 v = hemiloc(r1,z);

  EXPECT_NEAR(.1, v.z, .001f);
}

TEST(orthonizeTests, o1test0) {
  Vector3 x(1,0,0);
  pcg32_state rng = init_pcg32(time(NULL));
  Real r0 = next_pcg32_real<Real>(rng);
  Vector3 y(r0,sqrt(1-r0*r0),0.);
  Vector3 orth = orthonize(x,y);
  EXPECT_NEAR(0,dot(orth,y), .001);
}

TEST(SphereTests, spdfValueTest0) {
  Sphere sph;
  sph.center = Vector3(0,0,0);
  sph.radius = Real(2);
  Vector3 o(4,0,0);
  auto p = sph.pdf_value(o,o);
  EXPECT_NEAR(c_PI *(2 - sqrt(3)), 1/p, .001f);
}

TEST(SphereTests, spdfValueTest1) {
  Sphere sph;
  sph.center = Vector3(0,0,0);
  sph.radius = Real(2);
  Vector3 o(2,0,0);
  auto p = sph.pdf_value(o,o);
  EXPECT_NEAR(c_PI * 2, 1/p, .001f);
}

TEST(SphereTests, spdfValueTest2) {
  Sphere sph;
  sph.center = Vector3(0,0,0);
  sph.radius = Real(2);
  Vector3 o(1000,0,0);
  auto p = sph.pdf_value(o,o);
  EXPECT_NEAR(0, 1/p, .001f);
}

TEST(RandomToSphereTests, randomToSphere0) {
  Real distance_squared = Real(4.0f);
  Real area = Real(0);
  pcg32_state rng;
  Sphere sph;
  sph.center = Vector3(0,0,0);
  sph.radius = Real(2);
  auto v = sph.random_to_sphere(distance_squared, area, rng);
  EXPECT_NEAR(0, v.x, .001f);
  EXPECT_NEAR(0, v.y, .001f);
  EXPECT_NEAR(2, v.z, .001f);
  EXPECT_NEAR(0, area, .0001f);
}

TEST(RandomToSphereTests, randomToSphere1) {
  Real distance_squared = Real(16.0f);
  pcg32_state rng = init_pcg32(time(NULL));
  Real area = Real(0);
  Sphere sph;
  sph.center = Vector3(0,0,0);
  sph.radius = Real(2);
  auto v = sph.random_to_sphere(distance_squared, area, rng);
  EXPECT_NEAR(4, v.x * v.x + v.y * v.y + v.z * v.z, .001f);
  EXPECT_TRUE(1 < v.z);
  EXPECT_NEAR(4* c_PI, area, .0001f);
}

TEST(RandomToSphereTests, AreaTest) {
  Real distance_squared = Real(4.0f);
  Real area = Real(0);
  pcg32_state rng;
  Sphere sph;
  sph.center = Vector3(0,0,0);
  sph.radius = Real(2);
  auto v = sph.random_to_sphere(distance_squared, area, rng);
  EXPECT_NEAR(0, area, .0001f);
}

TEST(TriangleTests, AreaTest) {
  auto p0 = Vector3(0,0,0);
  auto p1 = Vector3(1,0,0);
  auto p2 = Vector3(0,2,0);
  auto n0 = Vector3(0,0,1);
  auto n1 = Vector3(0,0,1);
  auto n2 = Vector3(0,0,1);
  TriangleMesh mesh;
  mesh.positions.push_back(p0);
  mesh.positions.push_back(p1);
  mesh.positions.push_back(p2);
  mesh.normals.push_back(n0);
  mesh.normals.push_back(n1);
  mesh.normals.push_back(n2);
  Vector3i vi(0,1,2);

  mesh.indices.push_back(vi);
  Triangle tri{0,&mesh};
  tri.setup();
  EXPECT_NEAR(0, tri.crossproduct.x, .0001f);
  EXPECT_NEAR(0, tri.crossproduct.y, .0001f);
  EXPECT_NEAR(2, tri.crossproduct.z, .0001f);
}

TEST(SphericalTriangleTests, OrthonizeTest1) {
  Vector3 x(1,0,0);
  Vector3 y(0,1,0);
  auto orthed = orthonize(x,y);
  EXPECT_EQ(1, orthed.x);
  EXPECT_EQ(0, orthed.y);
  EXPECT_EQ(0, orthed.z);
}

TEST(SphericalTriangleTests, STAreaTest) {
  auto A = Vector3(1,0,0);
  auto B = Vector3(0,1,0);
  auto C = Vector3(0,0,1);

  SphericalTriangle st = spherical_triangle(A,B,C);

  EXPECT_NEAR(c_PI/2.0, st.area, .0001f);
  EXPECT_NEAR(0, st.cosines.x, .0001f);
  EXPECT_NEAR(0, st.cosines.y, .0001f);
  EXPECT_NEAR(0, st.cosines.z, .0001f);
  EXPECT_NEAR(0, st.cos_c, .0001f);
  EXPECT_NEAR(c_PI/2, st.area, .0001f);

}

TEST(SphericalTriangleTests, STAreaTest1) {
  auto A = Vector3(1/sqrt(2),1/sqrt(2),0.);
  auto B = Vector3(0,1,0);
  auto C = Vector3(0,0,1);

  SphericalTriangle st = spherical_triangle(A,B,C);

  EXPECT_NEAR(c_PI/4.0, st.area, .0001f);
  EXPECT_NEAR(0, st.cosines.x, .0001f);
  EXPECT_NEAR(0, st.cosines.y, .0001f);
  EXPECT_NEAR(1/sqrt(2), st.cosines.z, .0001f);
  EXPECT_NEAR(1/sqrt(2), st.cos_c, .0001f);
  EXPECT_NEAR(c_PI/4, st.area, .0001f);
}

TEST(RandomFrom_SphereTests, rightAngleTest) {
  auto p0 = Vector3(3,0,0);
  auto p1 = Vector3(0,3,0);
  auto p2 = Vector3(0,0,3);
  auto n0 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  auto n1 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  auto n2 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  TriangleMesh mesh;
  mesh.positions.push_back(p0);
  mesh.positions.push_back(p1);
  mesh.positions.push_back(p2);
  mesh.normals.push_back(n0);
  mesh.normals.push_back(n1);
  mesh.normals.push_back(n2);
  Vector3i vi(0,1,2);

  mesh.indices.push_back(vi);
  Triangle tri{0,&mesh};
  tri.setup();
  Vector3 c(0,0,0);
  pcg32_state rng = init_pcg32(time(NULL));
  auto r = tri.random_from_sphere(c, rng);
  EXPECT_NEAR(1, r.x * r.x + r.y* r.y + r.z * r.z, .0001f);
  EXPECT_TRUE(r.x > 0);
  EXPECT_TRUE(r.y > 0);
  EXPECT_TRUE(r.z > 0);
}

TEST(RandomFrom_SphereTests, halfAngleTest) {
  auto p0 = Vector3(3./2,3./2,0.);
  auto p1 = Vector3(0,3,0);
  auto p2 = Vector3(0,0,3);
  auto n0 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  auto n1 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  auto n2 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  TriangleMesh mesh;
  mesh.positions.push_back(p0);
  mesh.positions.push_back(p1);
  mesh.positions.push_back(p2);
  mesh.normals.push_back(n0);
  mesh.normals.push_back(n1);
  mesh.normals.push_back(n2);
  Vector3i vi(0,1,2);

  mesh.indices.push_back(vi);
  Triangle tri{0,&mesh};
  tri.setup();
  Vector3 c(0,0,0);
  pcg32_state rng = init_pcg32(time(NULL));
  auto r = tri.random_from_sphere(c, rng);

  EXPECT_NEAR(1, r.x * r.x + r.y* r.y + r.z * r.z, .0001f);
  EXPECT_TRUE(r.x > 0);
  EXPECT_TRUE(r.z > 0);
  EXPECT_TRUE(0 < r.y - r.x);
}

TEST(RandomtriTests, halfAngleTest) {
  Vector3 o(3,0,0);
  auto p0 = Vector3(3./2,3./2,0.);
  auto p1 = Vector3(0,3,0);
  auto p2 = Vector3(0,0,3);
  p0 +=o;
  p1 +=o;
  p2 +=o;
  auto n0 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  auto n1 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  auto n2 = Vector3(1/sqrt(3),1/sqrt(3),1/sqrt(3));
  TriangleMesh mesh;
  mesh.positions.push_back(p0);
  mesh.positions.push_back(p1);
  mesh.positions.push_back(p2);
  mesh.normals.push_back(n0);
  mesh.normals.push_back(n1);
  mesh.normals.push_back(n2);
  Vector3i vi(0,1,2);

  mesh.indices.push_back(vi);
  Triangle tri{0,&mesh};
  tri.setup();
  Vector3 n_x(0,0,0);
  Real area = 0;
  pcg32_state rng = init_pcg32(time(NULL));

  auto x = tri.random(o, n_x, area, rng);
  x= x - o;
  EXPECT_NEAR(1, x.x * x.x + x.y* x.y + x.z * x.z, .0001f);
  EXPECT_TRUE(x.x > 0);
  EXPECT_TRUE(x.z > 0);
  EXPECT_TRUE(0 < x.y - x.x);
}
