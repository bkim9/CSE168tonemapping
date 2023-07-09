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


add_subdirectory(src/3rdparty)
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
         src/parse_obj.cpp
         src/parse_ply.cpp
         src/parse_scene.cpp
         src/parse_serialized.cpp
         src/print_scene.cpp
         src/scene.cpp
         src/shape.cpp
         src/transform.cpp)

add_library(torrey_lib STATIC ${SRCS})


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(torrey src/main.cpp)

target_include_directories(torrey_lib PUBLIC include)
target_link_libraries(torrey_lib BilateralFilter ColorBilateralFilter LogMap)
target_link_libraries(torrey Threads::Threads 
  third_party_lib
  torrey_lib)         

