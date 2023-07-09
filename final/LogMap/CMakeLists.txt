add_library(LogMap log_tone.cpp)
target_include_directories(LogMap PUBLIC include)

add_library(color rgbe.cpp logmapping.cpp color_convert.cpp)
target_include_directories(color PUBLIC include)

target_link_libraries(LogMap color)
