add_library(ColorBilateralFilter STATIC color_bilateral_filter.cpp)
target_include_directories(ColorBilateralFilter PUBLIC include)
target_link_libraries(ColorBilateralFilter PUBLIC fft3d)