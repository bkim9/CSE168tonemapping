cmake_minimum_required(VERSION 3.12)
project(tone2original)

add_subdirectory(include/fft_3D)
find_package(OpenEXR REQUIRED)

add_executable(tone2ori tone_mapping.cpp)
target_link_libraries(tone2ori OpenEXR::OpenEXR fft3d)
target_include_directories(tone2ori PUBLIC include include/fft_3D)




