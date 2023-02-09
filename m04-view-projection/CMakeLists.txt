cmake_minimum_required(VERSION 3.17)
project(m03_view_projection)


set(CMAKE_CXX_STANDARD 20)

include_directories(C:/cs4722sp23lib/include lib64/include)
link_directories(C:/cs4722sp23lib lib64)
link_libraries(cs4722  glfw3 opengl32 glu32)

configure_file(fragment_shader.glsl .)
configure_file(vertex_shader.glsl .)


add_executable(00-shapes 00-shapes/shapes.cpp)
#add_executable(01-camera 01-view/animation-with-camera.cpp)
add_executable(01-view 01-view/view.cpp)
#add_executable(02-projection-old 02-projection/view-and-projection.cpp)
add_executable(02-projection 02-projection/projection.cpp)
add_executable(03-user-interaction 03-user-interaction/user-interaction.cpp)
add_executable(04-window-events 04-window-events/window_events.cpp)
#add_executable(05-callbacks-util 05-callbacks-util/callbacks_util.cpp)

#add_executable(multiple_spheres 02-projection/view-and-projection_multiple_spheres.cpp)
#add_executable(multiple_spheres_2 02-projection/view-and-projection_multiple_spheres_2.cpp)
#add_executable(camera_and_spheres 01-view/camera_and_spheres.cpp)

