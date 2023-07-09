cmake_minimum_required(VERSION 3.11)

project(lajolla)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# SET(CMAKE_CXX_FLAGS "-lm -lfftw3")
set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_BUILD_TYPE Debug)   

if(MSVC)
  add_compile_options(/Wall)
else()
  add_compile_options(-Wall)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src )



set(SRCS src/3rdparty/miniz.h
         src/3rdparty/miniz.c
         src/3rdparty/pugiconfig.hpp
         src/3rdparty/pugixml.hpp
         src/3rdparty/pugixml.cpp
         src/3rdparty/stb_image.h
         src/3rdparty/tinyexr.h
         src/3rdparty/tinyply.h
         src/bbox.h
         src/bvh.h
         src/camera.h
         src/compute_normals.h
         src/flexception.h
         src/hwfinalproject.h
         src/array_n.h
         src/array.h
         src/fast_color_bf.h
         src/geom.h
         src/math_tools.h
         src/mixed_vector.h
         src/color_bilateral_filter.h
         src/bilateral_filter.h
         src/fft_3D/convolution_3D.h
         src/fft_3D/fill_3D.h
         src/fft_3D/support_3D.h
         src/fft_3D/support_3D.cpp
         src/fft_3D.h
         src/log_tone.h
         src/color_convert.h
         src/logmapping.h
         src/rgbe.h
         src/log_SCENE.h
         src/image.h
         src/intersection.h
         src/ray.h
         src/material.h
         src/matrix.h
         src/parallel.h
         src/parse_obj.h
         src/parse_ply.h
         src/parse_scene.h
         src/parse_serialized.h
         src/pcg.h
         src/print_scene.h
         src/shape.h
         src/sphere.h
         src/torrey.h
         src/transform.h
         src/vector.h
         src/bvh.cpp
         src/camera.cpp
         src/compute_normals.cpp
         src/color_bilateral_filter.cpp
         src/bilateral_filter.cpp
         src/log_tone.cpp
         src/logmapping.cpp
         src/color_convert.cpp
         src/rgbe.cpp
         src/hwfinalproject.cpp
         src/image.cpp
         src/main.cpp
         src/material.cpp
         src/parallel.cpp
         src/parse_obj.cpp
         src/parse_ply.cpp
         src/parse_scene.cpp
         src/parse_serialized.cpp
         src/print_scene.cpp
         src/scene.cpp
         src/shape.cpp
         src/transform.cpp)

add_library(torrey_lib STATIC ${SRCS})

# Fourier transform --------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
# ----------------------- Fourier Transform -------------------





set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

add_executable(torrey src/main.cpp)

target_link_libraries(torrey Threads::Threads)

target_link_libraries(torrey_lib PkgConfig::FFTW)

target_link_libraries(torrey torrey_lib)



# target_link_libraries(torrey m)                   

