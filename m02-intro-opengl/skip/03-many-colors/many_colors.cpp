

/*
 * This example is very similar to the previous example, just with colors added.
 * So, the focus of the comments will be on handling and using colors.
 *
 * Try running this program using different drawing modes and observing how the colors are used.
 *
 *
 */


#include <glad/gl.h>
#include <GLM/glm.hpp>
#include <GLFW/glfw3.h>

/*
 * As a convenience, the local library includes a class representing colors.
 * Also, many of the common color names are interpreted as predfined color objects.
 * The names and values for these colors are mostly taken from the
 *      standard list of named colors for the X11 windows system,
 *      used in Unix and Linux.
 */
#include "cs4722/x11.h"
#include "learnopengl/shader.h"

// #include "cs4722/color.h"


/*
 * The number of vertices will need to be six.
 * The list of colors defined in the model below uses an initializer of length 6.
 */
const auto  number_of_vertices = 6;

void
init1(void)
{

    Shader shader("vertex_shader03.glsl","fragment_shader03.glsl" );
    auto program = shader.ID;
    shader.use();

    glPointSize(25.0);

    
    const auto delta = 2 * M_PI / number_of_vertices;
    const auto radius = .8f;
    glm::vec4 positions[number_of_vertices];
    for (auto i = 0; i < number_of_vertices; i++)
    {
        auto angle = i * delta;
        positions[i] = glm::vec4(static_cast<float>(radius * cos(angle)),
            static_cast<float>(radius * sin(angle)),
            0.0,
            1.0);
    }

    /*
     * We create a list of colors.
     * This is a list of color objects, not a list of pointers to objects.
     * This means that the data of all the colors is stored consecutively in memory.
     */
    const cs4722::color colors[number_of_vertices] = { cs4722::x11::white,
       cs4722::x11::blue, cs4722::x11::black, cs4722::x11::yellow,
       cs4722::x11::gray, cs4722::x11::orange,
    };

    // Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);



    /*
     * Two buffers in this example
     */
    const auto number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
     glGenBuffers(number_of_buffers, buffers);

     const int bPosition = glGetAttribLocation(program, "bPosition");
     /*
      * The second buffer is associated with the vertex shader variable bColor.
      */
     const int bColor = glGetAttribLocation(program, "bColor");

	
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//    glBufferStorage(GL_ARRAY_BUFFER, number_of_vertices*4*4, positions, 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, positions,  GL_STATIC_DRAW);
    glVertexAttribPointer(bPosition, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(bPosition);

    /*
     *  A color objects contains four bytes used to represent four color components
     *      of red, green, blue, and alpha.
     *  Each component has a value in the range 0 to 255.
     */
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    /*
     * Notice that the size of this buffer is one fourth the size of the positions buffer.
     * This is because each component of the color is one byte rather than the four bytes
     *      needed for a float value used in positions.
     */
//    glBufferStorage(GL_ARRAY_BUFFER, number_of_vertices * 4 * 1, colors, 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * number_of_vertices, colors,  GL_STATIC_DRAW);
    /*
     * This function call associates the buffer to the bColor variable.
     * There are four components per color value.
     * Each component is an unsigned byte.
     *
     * The fourth parameter is true here: the values should be normalized.
     * This means that the value of each component provided to the vertex shader is going
     *      to be a float value in the range 0.0 to 1.0.
     *      This value will be computed by dividing each byte value by the maximum possible, 255.
     *      Note that this is not an integer division but done as a floating point division in
     *      order to retain the fractional part.
     *  This normalization converts the color to the representation expected by the shaders.
     */
    glVertexAttribPointer(bColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);
    glEnableVertexAttribArray(bColor);


}

//----------------------------------------------------------------------------
//
// display
//

void
display(void)
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float_up().get());

//     glDrawArrays(GL_POINTS, 0, number_of_vertices);
     glDrawArrays(GL_LINES, 0, number_of_vertices);
    // glDrawArrays(GL_LINE_STRIP, 0, number_of_vertices);
    // glDrawArrays(GL_LINE_LOOP, 0, number_of_vertices);
    // glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, number_of_vertices);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, number_of_vertices);
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


    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Many Points", NULL, NULL);
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
