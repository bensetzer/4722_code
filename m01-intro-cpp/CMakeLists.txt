cmake_minimum_required(VERSION 3.22)
project(m01_intro_cpp)

set(CMAKE_CXX_STANDARD 14)

include_directories(C:/cs4722sp23lib/include lib64/include)
link_directories(/cs4722sp23lib lib64)
link_libraries(cs4722 glfw3 opengl32 glu32)


add_executable(01-hello-world 01-hello-world/main.cpp)

add_executable(03-one-triangle 03-one-triangle/one-triangle.cpp)
configure_file(03-one-triangle/fragment_shader03.glsl .)
configure_file(03-one-triangle/vertex_shader03.glsl .)

add_executable(02-window 02-window/window.cpp)