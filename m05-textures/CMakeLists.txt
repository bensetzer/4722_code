cmake_minimum_required(VERSION 3.17)
project(m04_textures)



link_libraries(cs4722  glfw3 opengl32 glu32)

set(CMAKE_CXX_STANDARD 20)

include_directories(C:/cs4722fa22lib/include lib64/include)
link_directories(C:/cs4722fa22lib lib64)

add_executable(01-texture-from-pattern 01-texture-from-pattern/computed-image.cpp)
configure_file(01-texture-from-pattern/vertex_shader01.glsl .)
configure_file(01-texture-from-pattern/fragment_shader01.glsl .)

add_executable(02-texture-from-image 02-texture-from-image/file-image.cpp)
#configure_file(02-texture-from-image/vertex_shader02.glsl .)
#configure_file(02-texture-from-image/fragment_shader02.glsl .)

add_executable(03-texture-on-shape 03-texture-on-shape/texture_on_shape.cpp)
configure_file(03-texture-on-shape/vertex_shader03.glsl .)
configure_file(03-texture-on-shape/fragment_shader03.glsl .)

add_executable(04-multiple-textures 04-multiple-textures/multiple_textures.cpp)

add_executable(05-min-filters 05-min-filters/min-filters.cpp)

add_executable(06-mixed-colors 06-mixed-colors/mixed-colors.cpp)
configure_file(06-mixed-colors/fragment_shader06.glsl .)
configure_file(06-mixed-colors/vertex_shader06.glsl .)
