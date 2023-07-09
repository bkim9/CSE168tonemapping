add_subdirectory(include/fft_3D)

add_library(BilateralFilter STATIC bilateral_filter.cpp)
target_include_directories(BilateralFilter PUBLIC include include/fft_3D)
target_link_libraries(BilateralFilter PUBLIC fft3d)