
/*
 * Applying transformations to the positions of vertices is a process that is central tod
 *      defining a scene.
 *  We start with an example in which we apply a fixed transformation to the vertices of a shape
 *      and observe the outcome.
 *  Vertex positions will be transformed in the vertex shader.
 *
 *  It is possible to apply transformations to the vertex positions
 *  in the driver code.
 *  This would be inefficient since the graphics system can potentially speed up the computations by
 *  the parallelism provided by modern graphics cards.
 *  Many executions of the vertex shader can be run in parallel thus multiplying the speed of the
 *  computation.
 *  Also, the transformed position data would have to be stored in the buffers and this would
 *  take time.
 *
 *  Doing the transformation to a position in the vertex shader means that the position data
 *      in the buffer will not change.
 *  We will, however, have to send the data describing the transformation to the graphics system.
 *  The data will be a 4 by 4 matrix, 16 floating numbers.
 *
 *  The mechanism we will use to transfer the matrix values from the driver to the vertex
 *      shader is a 'uniform' variable.
 *      Such a variable is given a value by the driver program and that value many not
 *      be changed during the course of a single call to drawArrays.
 *   Using a uniform variable is somewhat simpler than using a buffer.
 *   First, the location of the variable is determined.
 *   Second, an appropriate function is called to transfer the value to the uniform variable.
 *
 *   Because the location of a uniform variable does not vary during the program,,
 *      we compute that in the init() function.
 *   The uniform variable will be given its value in the display program.
 *   This allows for the value of the variable to change over the course of the execution of the program.
 *
 *  One other change is made in this program.
 *  Instead of writing out all the code for setting up and sizing a window,
 *      the code is gathered into a function that we call in function main().
 *
 *
 */


//   4x4 matrix functions are defined in this header
#include "GLM/mat4x4.hpp"
//  certain special matrices are computed by functions described in this header
#include "GLM/ext/matrix_transform.hpp"
//  we need a way to locate the matrix data in memory so that it can be stored in the
// uniform variable.  The function we use is described in this header.
#include "GLM/gtc/type_ptr.hpp"


#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/x11.h"
#include "cs4722/shape.h"


static GLuint  number_of_vertices;
static GLuint program;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;

void
init(void)
{

    /*
     * The first nine examples use the same shaders.
     */
    Shader shader("vertex_shader-m02.glsl" ,"fragment_shader-m02.glsl" );
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);

    // create a shape
//    cs4722::shape *b =  new cs4722::cylinder(.05, 20);
//    cs4722::shape* b = new cs4722::cylinder();
//    cs4722::shape* b = new cs4722::sphere(15,50);
//    cs4722::shape* b = new cs4722::sphere(4, 5);
//    cs4722::shape* b = new cs4722::block();
    cs4722::shape* b = new cs4722::torus();

    
    number_of_vertices = b->get_size();

	auto* positions = b->positions();
    auto* colors = b->colors();


    // Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);


    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, positions->data(),  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT,
        GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * number_of_vertices, colors->data(),  GL_STATIC_DRAW);
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(b_color);

    delete positions;
    delete colors;

}


void render(void)
{


    /*
     * We create a transformation as a product of special forms: translation times rotation times scale.
     */
    /*
     * Start with the identity matrix.
     * call glm::mat4 with a single parameter creates a diagonal matrix with the parameter value
     *      on the diagonal.
     */
    glm::mat4 tr = glm::mat4(1.0f);
    /*
     * Multiply tr by a translation matrix and store the result back in tr.
     * The translation is .5 in the x direction, .5 in the y direction and none in the z direction.
     *
     * The comment on the line shows the multiplication in symbolic form.
     */
    tr = glm::translate(tr, glm::vec3(.5f, -.5f, 0.0f));  // tr = tr * translate
    // tr = glm::rotate(tr, -M_PI / 4, glm::vec3(0, 0, 1));
    // tr = glm::rotate(tr, -M_PI / 4, glm::vec3(0, 1, 0));
    /*
     * Multiply tr by a rotation matrix and assign the result back to tr.
     * The second parameter to rotate is the angle to rotate.
     * THe third parameter is the axis of rotation.
     * If the axis vector is pointing towards us a positive angle of rotation causes a
     *      counter-clockwise rotation.
     */
    tr = glm::rotate(tr, -(float)M_PI / 4, glm::vec3(1, 0, 0)); // tr = tr * translate * rotate
//    tr = glm::rotate(tr, -(float)M_PI / 4, glm::vec3(0, 1, 0)); // tr = tr * translate * rotate
    /*
     * Multiply a scale matrix.
     */
    tr = glm::scale(tr, glm::vec3(.2f, .5f, .5f));  // tr = tr * translate * rotate * scale
    // //std::cout << tr << std::endl;


    /*
     * send the data to the graphics system to be stored in the uniform variable.
     *
     * The name of the function indicates that the value being sent is a 4 by 4 matix with
     *      floating entries.
     *  The values in the matrix are pointed to by the last parameter.
     *
     *  The function glm::value_ptr returns a pointer to the beginning of the data in tr.
     */
    glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(tr));


    glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
    
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto* primary = glfwGetPrimaryMonitor();
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
//    std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
    const auto size = glm::min(width, height);
    const auto ratio = .9f;
    const auto w_width = static_cast<int>(ratio * size);
    const auto w_height = w_width;
    const auto w_x = static_cast<int>(size * (1 - ratio) / 2);
    const auto w_y = w_x;
    auto* window = glfwCreateWindow(w_width, w_height, "Static Transformation", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    init();

    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
        glClear(GL_DEPTH_BUFFER_BIT);

        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
