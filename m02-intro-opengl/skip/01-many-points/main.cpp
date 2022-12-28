

/*
 *   As we move from example to example we will not usually repeat comments from previous examples
 *   for a part of the program that has not changed.
 */


#include <iostream>

#include <glad/gl.h>

/*
 * GLM contains mathematically oriented classes and functions.
 *  We will use glm::vec4 below.
 *  This include file also brings in the constant M_PI for the constant pi.
 *
 */
#include <GLM/glm.hpp>
#include "GLM/ext/scalar_constants.hpp"
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include <flecs.h>

//const auto M_PI = glm::pi<double>();

const auto  number_of_vertices = 5;


class Mesh {
public:
    glm::vec4* data;
};

static flecs::world ecs;
//static auto ent = ecs.entity();

void
init1(void)
{

    /*
     * Compile and use the shaders.
     */
    Shader shader("vertex_shader01.glsl","fragment_shader01.glsl");
    auto program = shader.ID;
    shader.use();

    // set the pont size
    glPointSize(25);




    auto ent = ecs.entity();

    Mesh ms;
    ms.data = new glm::vec4[number_of_vertices];
    ent.set<Mesh>(ms);

    const auto delta = 2 * M_PI / number_of_vertices;
    const auto radius = .8f;
    for (auto i = 0; i < number_of_vertices; i++)
    {
        auto angle = i * delta;
        ms.data[i] = glm::vec4(static_cast<float>(radius * cos(angle)),
            static_cast<float>(radius * sin(angle)), 
            0.0, 
            1.0);
    }


    /**
     *  As mentioned in Module 1, it is necessary to have a vertex array object bound.
     *  The VAO must be created and bound before creating and configuring vertex buffers.
     */
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    const auto number_of_buffers = 1;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, ent.get<Mesh>()->data,  GL_STATIC_DRAW);
    const auto b_position = glGetAttribLocation(program, "vPosition");
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(b_position);
}

//----------------------------------------------------------------------------
//
// display
//

void
display(void)
{

    static const float gray20[] = { 0.2f, 0.2f, 0.2f, 0.2f };

    glClearBufferfv(GL_COLOR, 0, gray20);


//    glDrawArrays(GL_POINTS, 0, number_of_vertices);
     glDrawArrays(GL_LINES, 0, number_of_vertices);
//    glDrawArrays(GL_LINE_STRIP, 0, number_of_vertices);
    // glDrawArrays(GL_LINE_LOOP, 0, number_of_vertices);
    // glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, number_of_vertices);
    // glDrawArrays(GL_TRIANGLE_FAN, 0, number_of_vertices);

}


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int
main(int argc, char** argv)
{

    glfwSetErrorCallback(error_callback);
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    /*
     * In the first example we considered, the size of the window was fixed.
     * This means that the window will look relatively smaller on higher
     *      resolution monitors.
     *  The code below sets the window size to about 90% of the smaller screen
     *      dimension and centers the window on that dimension.
     */

    /*
     * We need to find out how big the screen is.
     * We will assume we are using the primary monitor for this.
     */
    auto* primary = glfwGetPrimaryMonitor();
    /*
     * Variables are defined in which the function glfwGetMonitorWorkarea can
     * put the dimensions of the working area of the monitor.
     */
    int xpos, ypos, width, height;
    /*
     * Note that the addresses of the variables are passed to the function.
     * Tha means that the function can put values directly into the variables
     *      pointed to.
     *  This is how C++ and C simulate 'pass by reference'.
     */
    glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
    /*
     * Print out the values so we can verify they make sense.
     */
    std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
    /*
     * Compute the width and height of the window we will construct.
     * Also compute the position of the upper left corner of the window relative to the
     * upper left corner of the screen.
     */
    const auto size = glm::min(width, height);
    const auto ratio = .9f;
    const auto w_width = static_cast<int>(ratio * size);
    const auto w_height = w_width;
    const auto w_x = static_cast<int>(size * (1 - ratio) / 2);
    const auto w_y = w_x;


    /*
     * Create the window specifying the desired size.
     */
    auto* window = glfwCreateWindow(w_width, w_height, "Many Points", NULL, NULL);
    /*
     * Position the window
     */
    glfwSetWindowPos(window, w_x, w_y);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    init1();

    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
