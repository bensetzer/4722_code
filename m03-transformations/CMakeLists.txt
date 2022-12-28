cmake_minimum_required(VERSION 3.17)
project(m02-transformation)


link_libraries(cs4722 glfw3 opengl32 glu32)

set(CMAKE_CXX_STANDARD 20)

include_directories(c:/cs4722fa22lib/include lib64/include)
link_directories(c:/cs4722fa22lib lib64)

# add_compile_definitions(_GLFW_WIN32)


configure_file(fragment_shader-m02.glsl .)
configure_file(vertex_shader-m02.glsl .)

add_executable(01-static-transform 01-static-transform/static-transform.cpp )
add_executable(02-use-transform 02-use-transform/static-transform-use-transform.cpp)
add_executable(03-use-artifact-and-attributes 03-use-artifact-and-attributes/use_artifact.cpp)
add_executable(04-multiple-artifacts 04-multiple-artifacts/multiple_artifacts.cpp)
add_executable(04-multiple-artifacts-buffered 04-multiple-artifacts/multiple_artifacts_buffered.cpp)
add_executable(05-shared-shapes 05-shared-shapes/shared_shapes.cpp)
add_executable(05-shared-shapes-buffered 05-shared-shapes/shared_shapes_buffered.cpp)
#add_executable(06-buffer-utilities 06-buffer-utilities/shared_shapes_buffer_utilities.cpp)
add_executable(07-animation 07-animation/animation.cpp)
add_executable(08-rotate-about-center 08-rotate-about-center/rotate-about-center.cpp)
add_executable(09-oscillation 09-oscillation/oscillation.cpp 09-oscillation/artifact_oscillating.cpp)
add_executable(10-animation-many 10-animation-many/animation_many.cpp)
configure_file(11-use-material/fragment_shader-11.glsl .)
configure_file(11-use-material/vertex_shader-11.glsl .)

add_executable(11-use-material 11-use-material/use_material.cpp)
# uses shaders from 10

add_executable(12-mix-material-shape-pattern 12-mix-material-shape-pattern/mix_material_shape.cpp)
configure_file(12-mix-material-shape-pattern/fragment_shader-12.glsl .)
configure_file(12-mix-material-shape-pattern/vertex_shader-12.glsl .)
