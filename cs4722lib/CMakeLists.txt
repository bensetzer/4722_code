cmake_minimum_required(VERSION 3.17)
project(cs4722lib)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)


add_library(cs4722 STATIC )


target_sources(cs4722 PUBLIC src/cs4722/block.cpp
#		src/cs4722/buffer_utilities.cpp
#		src/cs4722/callbacks.cpp
		src/cs4722/cylinder.cpp
		#        src/cs4722/compile_shaders.cpp
		src/cs4722/artifact.cpp
		src/cs4722/artifact_elliptical_orbit.cpp
		src/cs4722/rectangle.cpp
		src/cs4722/shape.cpp
		src/cs4722/sphere.cpp
		src/cs4722/texture_utilities.cpp
		src/cs4722/torus.cpp
		src/cs4722/transform.cpp
		src/cs4722/utility_glm.cpp
		src/cs4722/view.cpp
#		src/cs4722/window.cpp
		src/cs4722/x11.cpp )

#target_sources(cs4722 PUBLIC
#		src/cs4722/window.cpp
#		src/cs4722/utility_glm.cpp
#		src/cs4722/transform.cpp
#		src/cs4722/x11.cpp)
        
target_sources(cs4722 PUBLIC
		src/glad/gl.c
        src/STB/stb_image.cpp
		src/redbook/LoadShaders.cpp
        )

link_directories(lib64)
# link_directories(lib-vs)
link_libraries(cs4722 glfw3 opengl32 glu32)

#add_executable(01-many-points 01-many-points/main.cpp)
#configure_file(01-many-points/fragment_shader01.glsl .)
#configure_file(01-many-points/vertex_shader01.glsl .)


add_executable(02-many-colors 02-many-colors/main.cpp)
configure_file(02-many-colors/fragment_shader02.glsl .)
configure_file(02-many-colors/vertex_shader02.glsl .)
