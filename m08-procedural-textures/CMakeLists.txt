cmake_minimum_required(VERSION 3.17)
project(m08_procedural_textures)


link_libraries(cs4722  glfw3 opengl32 glu32)

set(CMAKE_CXX_STANDARD 20)

include_directories(lib64/include C:/cs4722fa22lib/include)
link_directories(lib64 c:/cs4722fa22lib)

#file(GLOB glsls */*.glsl)
##file(COPY ${glsls} DESTINATION .)
#foreach(shader ${glsls})
#    configure_file(${shader} .)
#endforeach()


add_executable(01-stripes 01-stripes/stripes.cpp)
configure_file(01-stripes/fragment_shader01.glsl .)
configure_file(01-stripes/vertex_shader01.glsl .)

add_executable(02-fractal 02-fractal/fractal.cpp)
configure_file(02-fractal/fragment_shader02.glsl .)
configure_file(02-fractal/vertex_shader02.glsl .)


add_executable(03-bump-map 03-bump-map/bump_map.cpp)
configure_file(03-bump-map/fragment_shader03.glsl .)
configure_file(03-bump-map/vertex_shader03.glsl .)

add_executable(04-noise-on-square 04-noise-on-square/noise-on-square.cpp)
configure_file(04-noise-on-square/fragment_shader04.glsl .)
configure_file(04-noise-on-square/vertex_shader04.glsl .)

add_executable(05-compare-noise 05-compare-noise/compare_noise.cpp)
configure_file(05-compare-noise/fragment_shader05.glsl .)
configure_file(05-compare-noise/vertex_shader05.glsl .)

add_executable(06-clouds 06-clouds/clouds.cpp)
configure_file(06-clouds/fragment_shader06.glsl .)
configure_file(06-clouds/vertex_shader06.glsl .)

add_executable(07-clouds-glsl 07-clouds-glsl/clouds_glsl.cpp)
configure_file(07-clouds-glsl/fragment_shader07.glsl .)
configure_file(07-clouds-glsl/vertex_shader07.glsl .)
