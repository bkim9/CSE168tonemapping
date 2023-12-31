cmake_minimum_required(VERSION 3.11)
add_subdirectory(include/fft_3D)
find_package(OpenEXR REQUIRED)

add_library(ToneMap2 STATIC tonemapping2.cpp)
target_include_directories(ToneMap2 PUBLIC include include/fft_3D)
target_link_libraries(ToneMap2 PUBLIC OpenEXR::OpenEXR fft3d)


include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
add_executable( testapp
                bilateral_filter_unittest.cc)

target_include_directories(testapp PUBLIC include)

target_link_libraries(testapp PRIVATE ToneMap2 GTest::gtest_main)
target_compile_options(testapp PRIVATE ${GTEST_CFLAGS})

enable_testing()
add_test(first_and_only_test testapp)

