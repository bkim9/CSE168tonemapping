cmake_minimum_required(VERSION 3.11)

project(lajolla)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_BUILD_TYPE Debug)   

if(MSVC)
  add_compile_options(/Wall)
else()
  add_compile_options(-Wall)
endif()


add_subdirectory(src/parse)
add_subdirectory(TONEMAP1)
add_subdirectory(BILATERAL_FILTER)
add_subdirectory(COLOR_BILATERAL_FILTER)
add_subdirectory(LogMap)

set(SRCS src/bvh.cpp
         src/camera.cpp
         src/compute_normals.cpp
         src/hwfinalproject.cpp
         src/image.cpp
         src/material.cpp
         src/parallel.cpp
         src/print_scene.cpp
         src/scene.cpp
         src/shape.cpp
         src/transform.cpp)

add_library(torrey_lib STATIC ${SRCS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(torrey src/main.cpp)

target_include_directories(torrey_lib PUBLIC include)

target_link_libraries(torrey_lib 
  ToneMap1
  BilateralFilter 
  ColorBilateralFilter 
  LogMap 
  parse_lib) 

target_link_libraries(torrey 
  Threads::Threads 
  torrey_lib)         


# gtest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)


# shape test
enable_testing()
add_executable( testshape
                unittest/shape_unittest.cpp
)
target_include_directories(testshape PUBLIC include)
target_link_libraries(testshape PRIVATE torrey_lib GTest::gtest_main)
target_compile_options(testshape PRIVATE ${GTEST_CFLAGS})


include(GoogleTest)
gtest_discover_tests(testshape)


add_test(shape_test testshape)


# tonemap1 test
add_executable( testtone1
                unittest/tonemap1_unittest.cpp
)

target_include_directories(testtone1 PUBLIC TONEMAP1/include)
target_link_libraries(testtone1 PRIVATE torrey_lib GTest::gtest_main)
target_compile_options(testtone1 PRIVATE ${GTEST_CFLAGS})


# include(GoogleTest)
gtest_discover_tests(testtone1)


add_test(shape_test testtone1)

# # radiance test
add_executable( testradiance
                unittest/radiance_unittest.cpp
)

target_include_directories(testradiance PUBLIC include)
target_link_libraries(testradiance PRIVATE torrey_lib GTest::gtest_main)
target_compile_options(testradiance PRIVATE ${GTEST_CFLAGS})


include(GoogleTest)
gtest_discover_tests(testradiance)

add_test(radiance_test testradiance)

# # radiance test
add_executable( testpdf
                unittest/pdf_unittest.cpp
)

target_include_directories(testpdf PUBLIC include)
target_link_libraries(testpdf PRIVATE torrey_lib GTest::gtest_main)
target_compile_options(testpdf PRIVATE ${GTEST_CFLAGS})


include(GoogleTest)
gtest_discover_tests(testpdf)

add_test(pdf_test testpdf)