
/*
 *  The purpose of this example is to illustrate the orientation of the three axes.
 *  Other than changing the locations of the vertices and changing their colors, this is
 *      pretty much the same as the square example with depth testing added.
 *
 *  The example demonstrates x and y axes orientation using different colors for different vertices.
 *      Comparing the coordinates then indicates which way is positive and which way is negative.
 *
 *  The yellow triangle has a yellow vertex with x coordinate .9
 *      The light gray vertex of that triangle has x coordinate -.9.
 *      Since the yellow vertex is towards the right, that means the positive x-axi goes
 *          to the right.
 *  The blue triangle has a blue vertex with y coordinate .9 and a light gray vertex with
 *      y coordinate -.9.
 *      Since the blue coordinate is at the top, that means the positive y axis is
 *      pointing upward.
 *
 *
 *  The z axis is more difficult to show. In this example we depend on depth testing.
 *      The vertices of the blue triangle all have z coordinate -.5
 *      The vertices of the yellow triangle all have z coordinate .5
 *      Depth testing is enabled, so the nearer triangle will cover up the farther.
 *      The blue triangle is nearer, so that means the negative z-axis is pointing
 *          towards the viewer and the positive z-axis is pointing away.
 *
 *   The example also demonstrates depth testing at work.
 *   The blue triangle is drawn first, but it is closer to the viewer.
 *   If depth testing is turned off (try it, comment out the glEnable statement below) then the
 *      blue triangle is covered by the yellow triangle where they cross.
 *      This is the painter's algorithm: what is drawn first is covered by what is drawn later.
 */

#include <iostream>

#include <GLM/glm.hpp>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include "cs4722/x11.h"
#include "learnopengl/shader.h"



const GLuint  number_of_vertices = 6;
const auto b_position = 1;
const auto b_color = 2;

void init(void)
{
    /*
     * Using the same shaders as in the many colors example.
     */
    Shader shader("vertex_shader03.glsl","fragment_shader03.glsl");
    const auto program = shader.ID;
    shader.use();

    glPointSize(15.0);
    glEnable(GL_DEPTH_TEST);

  


    // blue in front at positive x
    // yellow in back at positive y
    // blue drawn first
   //  float  vertices[NumVertices][4] = {
   //     {.9, 0, .5, 1},
   // 	{-.9, 0, .5, 1},
   // 	{0, -.2, .5, 1},
   // 	{0, .9, -.5, 1},
   // 	{0, -.9, -.5, 1},
   // 	{.2, 0, -.5, 1}
   // };

    // blue negative z at positive y
    // yellow positive z at positive x
    // blue drawn first
    float  vertices[number_of_vertices][4] = {
   {0, .9, -.5, 1},
   {0, -.9, -.5, 1},
   {.2, 0, -.5, 1},
      {.9, 0, .5, 1},
   {-.9, 0, .5, 1},
   {0, -.2, .5, 1},
    };

    std::cout << "sizeof vertices " << sizeof(vertices) << std::endl;

    const cs4722::color colors[number_of_vertices] = {
            cs4722::x11::blue,
            cs4722::x11::gray75,
            cs4722::x11::gray25,
            cs4722::x11::yellow,
            cs4722::x11::gray75,
            cs4722::x11::gray25,
    };

    // Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const auto number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);


    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * number_of_vertices, colors,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);
    glEnableVertexAttribArray(b_color);


}

void render(void)
{
    glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
    glClear(GL_DEPTH_BUFFER_BIT);
	
   
    glDrawArrays(GL_POINTS, 0, number_of_vertices);
    //glDrawArrays(GL_LINES, 0, NumVertices);
    //glDrawArrays(GL_LINE_STRIP, 0, NumVertices);
    //glDrawArrays(GL_LINE_LOOP, 0, NumVertices);
    glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, NumVertices);
    // glDrawArrays(GL_TRIANGLE_FAN, 0, NumVertices);
}

int
main(int argc, char** argv)
{
    glfwInit();

    // set the OpenGL version to use: 4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
    //std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
    int size = glm::min(width, height);
    float ratio = .9f;
    int w_width = int(ratio * size);
    int w_height = w_width;
    int w_x = int(size * (1 - ratio) / 2);
    int w_y = w_x;


    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Coordinate Orientation", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    init();

    while (!glfwWindowShouldClose(window))
    {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
