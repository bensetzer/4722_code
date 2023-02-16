cmake_minimum_required(VERSION 3.17)
project(m06_lighting_and_shading)





link_libraries(cs4722  glfw3 opengl32 glu32)

set(CMAKE_CXX_STANDARD 20)

include_directories(lib64/include C:/cs4722sp23lib/include)
link_directories(lib64 C:/cs4722sp23lib)

file(GLOB glsls */*.glsl)
foreach(shader ${glsls})
    configure_file(${shader} .)
endforeach()

add_executable(01-ambient-color 01-ambient-color/ambient_color.cpp)

add_executable(02-ambient-light 02-ambient-light/ambient-light.cpp)

add_executable(03-visualize-normals 03-visualize-normals/visualize_normals.cpp)

add_executable(04-adjust-normals 04-adjust-normals/adjust_normals.cpp)

add_executable(06-directional-lighting
        06-directional-lighting/directional_lighting.cpp)

add_executable(05-point-lighting
        05-point-lighting/point_lighting.cpp)

add_executable(07-shading-textures
        07-shading-textures/shading_textures.cpp)