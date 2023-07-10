add_subdirectory(3rdparty)
add_library(parse_lib parse_obj.cpp parse_ply.cpp parse_scene.cpp parse_serialized.cpp)
target_include_directories(parse_lib PUBLIC include ../../include ../../BILATERAL_FILTER/include)
target_link_libraries(parse_lib third_party_lib)