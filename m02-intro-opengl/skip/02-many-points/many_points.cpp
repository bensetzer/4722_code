

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
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"




const auto  number_of_vertices = 7;

void
init1(void)
{

    /*
     * Compile and use the shaders.
     */
    Shader shader("vertex_shader02.glsl","fragment_shader02.glsl");
    auto program = shader.ID;
    shader.use();

    // set the pont size
    glPointSize(25);


    /*
     * In almost all of our programs init will first define a 'model' and then send the data for that
     * model to the graphics system.
     *
     * A model consists of a collection of vertices that are used to define primitives.
     * Vertices can have multiple attributes but almost always have a position for the vertex.
     *
     * In this program, each vertex has a position.  We are not defining any other attributes for
     * each vertex.
     *
     * The model here is a set of 'number_of_vertices' points distributed evenly around a circle centered
     * at the origin.
     * The radius of the circle is given by the variable 'radius'.
     */
    const auto delta = 2 * M_PI / number_of_vertices;
    const auto radius = .8f;
    // the array 'positions' will hold the positions of the various vertices.
    glm::vec4 positions[number_of_vertices];
    for (auto i = 0; i < number_of_vertices; i++) 
    {
        auto angle = i * delta;
        /*
         * We use a 'vec4', a 4-tuple of floating point numbers, to represent each position.
         * The reason for using a 'vec4' will be explained in the module 'out of the box' which discusses
         *  projections.
         *  For positions, the fourth component of each vec4 is set to 1.0.
         */
        positions[i] = glm::vec4(static_cast<float>(radius * cos(angle)), 
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



    /*
     * The model consists of the 'positions' vector.
     * It is now time to send that data to the graphics system.
     * We will use a buffer which will provide data to the vertex shader.
     *
     * The comments in the vertex shader file will explain how the vertex data is used.
     */
    /*
     * Only one buffer is needed since we are only providing positions in this manner.
     */
    const auto number_of_buffers = 1;
    /*
     * Define an array to hold the buffer values.
     * These are just integers that OpenGL uses to identify each buffer created.
     */
    GLuint  buffers[number_of_buffers];
    /*
     * Generate the buffers.
     *
     * The first parameter is the number of buffers to create.
     *
     * The second parameter is a pointer the first element of an array of integers
     *  where the buffer idenifiers will be stored.
     */
    glGenBuffers(number_of_buffers, buffers);

    /*
     * Now data must be put in the buffer and the structure of the data must be described.
     *
     * Note that a buffer simply contains a sequence of bytes.  We must explicitly describe how
     *      the graphics system should interpret those bytes.
     */
    /*
     *  One way of approaching this task is to bind the buffer to a 'binding point'.
     *  This allows us to operate on one buffer at a time.
     *  This approach has the advantage of being compatible with earlier versions of OpenGL.
     *
     *  The first parameter to glBindBuffer is a symbolic constant that specifies the binding point.
     *
     *  The second parameter to glBindBuffer is the buffer identifier.
     *      In this case the buffer identifier is the first (and only) element in the array 'buffers'.
     */
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	/*
	 * Next put the data into the buffer.
	 *
	 * The first parameter to glBufferStorage is the symbolic constant specifying the binding point.
	 *
	 * The second parameter is the number of bytes taken up by the data.
	 *      Each vertex has a position  consisting of four floating point numbers.
	 *      Each floating point number takes four bytes.
	 *      Multiplying the number of vertices by 4*4 gives the number of bytes.
	 *
	 *  The third parameter points to the start of the data.
	 *      The name of an array is a pointer to the first element in the array
	 *      The data for a vec4 is stored consecutively in memory, so all the data
	 *          is in a single block of memory.
	 *
	 *  The fourth parameter is a 'skip' value.  Using 0 indicates that all the data is
	 *      in consecutive bytes in memory.
	 *      Our data will always be stored that way.
	 */
//    glBufferStorage(GL_ARRAY_BUFFER, number_of_vertices*4*4, positions, 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, positions,  GL_STATIC_DRAW);
    /*
     * The data from the buffer will be placed in a variable in the vertex shader.
     * One value is placed in that variable each time the vertex shader is executed.
     * THe purpose of glGetAttribLocation is to return an integer that identifies the
     *      location of the variable in memory.
     * The first parameter is the shader program identifier.
     * The second parameter is a string giving the name of the variable.
     *      We must be careful to spell the name correctly and match upper and lower case.
     *      The compiler will not check this for us.
     *
     *  Look in the vertex shader (vertex__shader02.glsl) to see the variable vPosition defined.
     */
    const auto b_position = glGetAttribLocation(program, "vPosition");
    /*
     * Now we describe the layout of the data.
     *
     * Notice that no mention is made of the buffer or the binding point in the parameters.
     * This function only works with the binding point GL_ARRAY_BUFFER.
     *
     * The first parameter is the identifier that specifies the position of the receiving variable in
     *      graphics memory.
     *  The second parameter specifies how many components each element has.
     *      In this case the value is 4 since a vec4 has four components.
     *
     *  The third parameter is a symbolic constant specifying the type of each component.  For the
     *      position data, each component is a float.
     *
     *  The fourth parameter is a symbolic boolean constant.  This specifies whether the data  provided should
     *      be normalized before giving to the vertex shader.
     *      For positions that is not necessary.
     *
     *  The last two parameters are not of interest to us, but we must provide suitable neutral values.
     */
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    /*
     * As we did with the program, we must enable the buffer in order for the graphics system to actually
     *  transfer data into the variable defined in the vertex shader.
     */
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

   /*
    *   The next few lines will draw primitives.
    *   Initially only the first line will actually execute, the others are
    *       commented out.
    *   Multiple drawing operations can be executed with the same set of vertices.
    *   The result combines the individual drawing operations.
    *   With the configuration of the OpenGL context we are using here, the
    *       later drawing will cover up earlier drawing if drawn in the
    *       same place.
    *
    *    Seven different drawing modes are illustrated here.
    *
    *    To get the best understanding of the different modes first try uncommenting one
    *       line at a time and see what the resulting image looks like.
    *
    *    Then, using the GL_TRIANGLES mode, try different numbers of vertices and see how many
    *       triangles are actually drawn.  This is the mode we will use in almost all our
    *       examples.  Three vertices are used to describe each triangle.
    *
    *   See the text for how each mode operates.
    *
    *   The third parameter in each function call is 'number_of_vertices'.
    *   This matches the number of vertices defined in the single buffer in function init.
    *   The vertex shader will be executed that number of times, so each vertex
    *       will be processed by the vertex shader.
    *
    */
    glDrawArrays(GL_POINTS, 0, number_of_vertices);
    // glDrawArrays(GL_LINES, 0, number_of_vertices);
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
