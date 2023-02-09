

/*
 *   All I ever needed to know I learned in Kindergarten.
 *   Well, maybe not for this course.
 *   However, this very first example is super important.
 *   This application is the basic framework we will use for all the examples in the course.
 *   Once you understand that parts of this application then you will be able to analyze more
 *   complex examples by seeing which part of the application they affect.
 *   For example, in the 'shapes' application we will change the way we generate vertex data
 *      letting us conveniently use shapes such as spheres and cylinders that are more
 *      complicated that triangles and squares.
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

#include "cs4722/x11.h"

/*
 * Three vertices are used to specify a single triangle.
 */
static const auto  number_of_vertices = 3;

/*
 * Location of the uniform variable used to communicate the triangle color to the
 * fragment shader.
 */
static const unsigned int color_loc = 1;

/*
 * Location of the buffer variable for the vertex position.
 * The variable is found in the vertex shader.
 */
const auto b_position = 1;

static const auto background_color = cs4722::x11::gray70;

void init(void)
{
    /*
     * Compile and use the shaders.
     * Compiling the shaders happens when this program runs.
     */
    Shader shader("vertex_shader03.glsl","fragment_shader03.glsl");
    auto program = shader.ID;
    if(program < 0) {
        printf("Error in shader compilation");
        /*
         * We halt the program if a shader syntax error is discovered.
         * Otherwise, even though there is such an error, the program may continue to run.
         * In this situation the window is usually blank.
         * Quite often there are errors that are reported during rendering.  Since rendering
         * is repeated many times per second, the later error messages will usually overflow the
         * 'Run' window so that you can't see the error message from the shader compilation.
         *
         */
        std::exit(-1);
    }
    shader.use();


    /*
     * Position will be the only vertex attribute used in this example.
     * There are three vertices so there are three positions.
     * Each position is specified using a vec4 from the GLM library.
     *
     * An important detail here is that the 12 float numbers that make up these
     * positions are adjacent in memory.
     */
    glm::vec4 positions[] = {
            glm::vec4(-.5, .8, 0, 1),
            glm::vec4(-.8, .2,0, 1),
            glm::vec4(-.2, .2, 0, 1)
    };


    /*
     * A Vertex Array Object, VAO, is used to group together information about buffers
     * and other OpenGL constructs.
     * The OpenGL specification requires a VAO be defined although some implementations
     * will not enforce that.
     * To be safe, we define a VAO and bind it.
     *
     * As with most other OpenGL objects a VAO is identified by an integer value.
     * The variable vao will hold the identifier for the vao created here.
     */
    GLuint vao[1];
    /*
     * The function glGenVertexArrays will generate one or more VAO's.
     * The first parameter specifies how many VAO's to create.
     * The second parameter is a pointer to an array that will hold the generated identifiers.
     * We're only using one VAO so use an array of length 1.
     */
    glGenVertexArrays(1, vao);
    /*
     * Only one VAO can be active at a time.
     * glBindVertexArray activates the VAO whose identifier is given as the parameter.
     */
    glBindVertexArray(vao[0]);
    /*
     * For the record, any buffers defined and set up from this point on, until another
     * VAO is bound, will belong to the VAO that was just bound.
     */

    /*
     * Buffers for vertex attributes are set up similarly to VAOs.
     * We need one buffer for positions.
     */
    const auto number_of_buffers = 1;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    /*
     * OpenGL has a somewhat confusing way of working with buffers.
     * When we wish to describe a buffer and put data into it we must first
     * bind the buffer to a suitable binding point.
     * The only binding point we will use for buffers is GL_ARRAY_BUFFER.
     */
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    /*
     * The next call allocates storage for the buffer and also puts data into the buffer.
     * Notice that the binding point is mentioned as the first parameter, not the buffer identifier.
     *      But, the buffer is bound to the binding point, so OpenGL knows which buffer
     *      is intended.
     * The second parameter is how many bytes are to be allocated.
     *      4 components for each position
     *      Each component is a float so takes 4 bytes
     *      There are 'number_of_vertices' positions
     * The third parameter is a pointer to the data in memory.
     *      It is important that the data be adjacent in memory, vertex by vertex.
     * The last parameter is a hint to OpenGL specifying that we do not intend to change the
     *      data in the buffer.
     */
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, positions,  GL_STATIC_DRAW);
    /*
     * Now we must describe the structure of the data in the buffer.
     * This is necessary because the buffer itself is simply a sequence of bytes in memory.
     * We must let the shaders know how these bytes are organized into meaningful data.
     *
     * Notice that neither the buffer nor the binding point are mentioned.
     * It is assumed that this will operate on the currently bound buffer.
     *
     * The first parameter is the location of the variable in the vertex shader that will
     *      receive this vertex attribute.
     *      The location is specified explicitly in the vertex shader and a symbolic constant
     *      holds the value for use here.
     *  The second parameter specifies how many components there are for this attribute.
     *      These are vec4's so we use 4 here.
     * The third parameter is a symbolic constant defined by OpenGL that specifies the type
     *      of each component.
     * The fourth component specifies no normalization.
     *      We will see how normalization works when assign colors as vertex attributes.
     * The last two components are used when the data in the buffer is not tightly packed.
     *      The data is packed in this example, so we use 0 and 0 (nullptr to keep the
     *      compiler happy).
     */
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    /*
     * Finally, we must enable the buffer.
     */
    glEnableVertexAttribArray(b_position);

    /*
     * Now each time the vertex shader is run successive vertex positions will be loaded
     * into the variable bPosition in the vertex shader.
     */
}


void render(void)
{
    /*
     *  We use the X11 library for the color names.
     */
    static const cs4722::color triangle_color = cs4722::x11::lime_green;
    /*
     * The function glUniform4fv sets the value of a uniform variable.
     *      This variable may be in either shader.
     *      In this example the uniform variable is in the fragment shader.
     *  4fv in the name describes the function
     *      4 means there are four values
     *      f means that the values are floats
     *      v means that the values will be provided as a single parameter
     *  The first parameter is the location of the uniform variable.
     *      This is set in the fragment shader and made available using a
     *      symbolic constant in the main program.
     *  The third parameter must be an array of four float values in adjacent memory.
     *      triangle_color itself is a color object
     *      as_float() returns a suitable array of four floats as an array in memory.
     *      get() returns a pointer to the start of the array.
     *
     *  In earlier instances of this course you will see that .get() is not used.
     *  Indeed, in the older versions of the library as_float() returned a pointer to
     *  the array directly.
     *  However, it turns out that introduces a memory leak.
     *      Every time that render is called, there is a new array allocated that is
     *      never cleaned up.
     *  The leak will not be evident in this program but does cause later programs,
     *  which use more colors, to crash after a couple of minutes.
     *  as_float() returns an object which will be deallocated automatically when the
     *  render function returns.
     */
    glUniform4fv(color_loc, 1, triangle_color.as_float().get());

    /*
     * This is the function that starts the rendering process, the pipleline.
     * The first parameter specifies that triangle primitives will be drawn.
     * This means that vertices will be used in groups of three to create triangles.
     * The second parameter specifies the vertex index to start at.
     *      0 means start with the first vertex in the buffer.
     * The third parameter specifies how many vertices to process.
     * Notice that both parameters are in terms of vertices, not bytes.
     */
    glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
}


/**
 * We'll discuss callback functions in more detail in a later module.
 * For now, it is enough to know that GLFW will call this function when GLFW  detects
 *      an error.
 *      GLFW will provide the two parameter values.
 * @param error An integer code for the error
 * @param description A human readable description of the error.
 */
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

/*
 * It may seem odd that this program is not structured like a Windows program.
 * However, even windows programs in C or C++ start with a main function.
 * It's just that the main function is pre-written to set up the windows environment
 *      and the programmer provides a subsidiary main that is called by the real main.
 * Here GLFW handles all the setting up of the windowing environment.
 * So, we provide main.
 */
int main(int argc, char** argv)
{
    /*
     * This function call 'registers' the function error_callback.
     * This is necessary to let GLFW know to call error_callback when there is an error.
     * Note carefully that the parameter is just the name of the function, there are
     *      no parentheses.  The function is not called here.
     */
    glfwSetErrorCallback(error_callback);
    /*
     * Initialize the GLFW library.
     * If that fails, glfwInit returns a 0 value which counts as false.
     * The program then exits by returning from main.
     *
     */
    if (!glfwInit())
        return -1;

    /*
     * Tell GLFW what version of OpenGL we are using.
     * This specifies 3.2
     *      major version is 3
     *      minor version is 3
     *  This should be ok for most current computers.
     */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    /*
     * The following is included in case we ever get this working on a Mac.
     * So far, though, no luck.
     */
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /*
     * The following code will create a window for graphics.
     * The code starts by using functions provided by GLFW to find out the size of the screen
     * available and then set the window to use a percentage of that size.
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
     *  This is how C simulates 'pass by reference'.
     */
    glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
    /*
     * Print out the values, so we can verify they make sense.
     */
    std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
    /*
     * Compute the width and height of the window we will construct.
     * The window will be square to match the default clipping area that will be rendered.
     * Also compute the position of the upper left corner of the window relative to the
     * upper left corner of the screen.
     */
    /*
     * Base the window size on the smaller of the two dimensions of the screen.
     */
    const auto size = glm::min(width, height);
    /*
     * This is the percentage (as a decimal) which the window will cover in the shorter
     * dimension.
     * A value for ratio of .9f means that the window height will be 90% the screen height in the
     * usual case where the screen is wider than it is tall.
     *
     * Try setting the ratio to .5f to see the smaller window.
     */
    const auto ratio = .9f;
    /*
     * Compute the width and height.
     * ratio times size is floating point so that must be converted to int.
     *
     * If you try setting w_height to half of w_width you will see that the scene is distorted.
     */
    const auto w_width = static_cast<int>(ratio * size);
    const auto w_height = w_width;
//    const auto w_height = w_width/2;   // try this instead of the previous line to see the distortion caused
                                        // by using a non-square window.
    /*
     * Now compute the location of the upper left corner of the window.
     * In the usual case where the height of the screen is less than the width, the window
     * will be centered up and down.
     */
    const auto w_x = static_cast<int>(size * (1 - ratio) / 2);
    const auto w_y = w_x;


    /*
     * Create the window specifying the desired size.
     */
    auto* window = glfwCreateWindow(w_width, w_height, "One Triangle", NULL, NULL);
    /*
     * Position the window
     */
    glfwSetWindowPos(window, w_x, w_y);

    /*
     * The window just created has an associated OpenGL context.
     * By making that context current all the OpenGL actions carried out from now on
     * will apply to that context and, therefore, the window.
     */
    glfwMakeContextCurrent(window);

    /*
     * This is a necessary task in order to be able to conveniently use OpenGL functionality in
     * the version we choose.
     * After using this, OpenGL functions can be called in the way functions are usually called..
     */
    gladLoadGL(glfwGetProcAddress);

    /**
     * Initialize the scene.
     */
    init();

    /*
     * The following loop is the heart of the program.
     * In some sense it is an infinite loop, it will keep iterating until the program finishes.
     *      The close button on an application window (the x in the upper right hand corner)
     *      does not force the application to close, it just signals the application that
     *      the user has asked to close.
     *      GLFW reacts to that event by setting a flag specifying that the window should
     *      close.
     *      The function glfwWindowShouldClose(window) then returns true which causes the
     *      loop to terminate.
     *          Note the loop keeps iterating as long as glfwWindowShouldClose(window)
     *          returns false!
     *          This is a common confusing pattern you'll find in many programming situations:
     *          keep going if you're not supposed to stop.
     *  The loop clears the window for drawing, renders the scene, and then takes care of
     *      some utility tasks.
     *      These are described in the loop code.
     */
    while (!glfwWindowShouldClose(window))
    {
        /*
         * Clear the window to the specified background color.
         */
        glClearBufferfv(GL_COLOR, 0, background_color.as_float().get());

        /*
         * Render the scene.
         */
        render();

        /*
         * Rendering is always done into the background buffer.
         * To make the image visible, swap the background buffer to the foreground and
         *      the foreground buffer to the background.
         *  This 'double buffering' prevents the user seeing artifacts such as half of the
         *      window being updated and half not.
         */
        glfwSwapBuffers(window);
        /*
         * Check if any events have happened that should be processed.
         */
        glfwPollEvents();
    }

    /*
     * It is always good to clean up.
     */
    glfwDestroyWindow(window);
    /*
     * Terminating GLFW allows GLFW to release any resources it was using.
     *
     * Don't assume that resources will be automatically recycled when a program ends.
     * As an extreme example, the original version of Microsoft Windows did not
     * automatically release
     * resources such as brushes and windows that were used in a program.
     * If the program did not clean up after itself and release those resources, running the program
     * over and over a few times would crash the OS.
     * While the current Windows OS does not suffer from that problem, there are still
     * problematic situations.
     * It is best to get in the habit of seeing to it yourself.
     */
    glfwTerminate();
}
