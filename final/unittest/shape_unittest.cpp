#include "shape.h"
#include <gtest/gtest.h>

TEST(hemilocTests, hemilocTestRadius) {
  Real r1 = 0.1;
  Real z = .1;
  Vector3 v = hemiloc(r1,z);
  
  EXPECT_EQ(1, length_squared(v));
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

TEST(RandomToSphereTests, randomToSphere) {
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