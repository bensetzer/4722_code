cmake_minimum_required(VERSION 3.17)
project(m01-data)

set(CMAKE_CXX_STANDARD 14)

include_directories(C:/cs4722fa22lib/include lib64/include)

#cmake_policy(VERSION 3.22)

link_directories(/cs4722fa22lib lib64)
link_libraries(cs4722 glfw3 opengl32 glu32)



add_executable(01-one-triangle 01-one-triangle/one-triangle.cpp)
configure_file(01-one-triangle/fragment_shader01.glsl .)
configure_file(01-one-triangle/vertex_shader01.glsl .)



add_executable(02-three-triangles 02-three-triangles/three-triangles.cpp)
configure_file(02-three-triangles/fragment_shader02.glsl .)
configure_file(02-three-triangles/vertex_shader02.glsl .)


add_executable(03-square 03-square/square.cpp)
configure_file(03-square/vertex_shader03.glsl .)
configure_file(03-square/fragment_shader03.glsl .)


add_executable(05-shapes 05-shapes/shapes.cpp)


add_executable(06-coordinate-orientation 06-coordinate-orientation/coordinate_orientation.cpp)


add_executable(04-square-one-buffer 04-square-one-buffer/square-one-buffer.cpp)


#add_executable(01-one-triangle 01-one-triangle/main.cpp)
#file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/01-one-triangle)
#configure_file(01-one-triangle/fragment_shader.glsl ./01-one-triangle)
#configure_file(01-one-triangle/vertex_shader.glsl ./01-one-triangle)
