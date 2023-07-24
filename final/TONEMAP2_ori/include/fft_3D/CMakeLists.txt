# Fourier transform --------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
# ----------------------- Fourier Transform ------ -------------

add_library(fft3d convolution_3D.h fill_3D.h support_3D.h support_3D.cpp msg_stream.h)

target_link_libraries(fft3d PkgConfig::FFTW)