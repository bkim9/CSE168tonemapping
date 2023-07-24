add_subdirectory(3rdparty)
add_library(parse_lib parse_obj.cpp parse_ply.cpp parse_scene.cpp parse_serialized.cpp)
target_include_directories(parse_lib PUBLIC include ../../include ../../TONEMAP2/include)
target_link_libraries(parse_lib third_party_lib)