/*
 * The only change in this program from example 01-static-transform is how the transformations are managed.
 *
 * We introduce a transform object defined in the cs4722 library.
 * This object stores the parameters for a rotation, for a scale, and for a translation.
 *      rotation:  angle of rotation (float) and axis of rotation (vec3)
 *      scale:  scale factors (vec3)
 *      translation: translation amounts (vec3)
 *
 *  A transform object has a method matrix() that returns a mat4x4 formed from the product of the
 *      translation times the rotation times the scale
 *  That is, the scale is applied first, then the rotation, and then the translation.
 *  This is a standard and intuitive way of describing transformations.
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

#include <cs4722/transform.h>
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

    Shader shader("vertex_shader-m02.glsl" ,"fragment_shader-m02.glsl" );
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);

    // create a shape
    cs4722::shape *b =  new cs4722::cylinder(.05, 20);
//    cs4722::shape* b = new cs4722::cylinder();
//    cs4722::shape* b = new cs4722::sphere(15,50);
//    cs4722::shape* b = new cs4722::sphere(4, 5);
//    cs4722::shape* b = new cs4722::block();
//    cs4722::shape* b = new cs4722::torus();

    
    number_of_vertices = b->get_size();

	auto* positions = b->positions();
    auto* colors = b->colors();

    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);


    // Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

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
     * Create the transform object
     */
    auto* tr = new cs4722::transform();
    /*
     * set the translation parameters: right by .5, down by .5, no movement nearer or farther
     */
    tr->translate = glm::vec3(.5f, -.5f, 0.0f);
    /*
     * Set the rotation parameters: a rotation of 90 degrees clockwise about the z axis.
     */
    tr->rotation_angle = -M_PI / 4;
    tr->rotation_axis = glm::vec3(0,0,1);
    /*
     * Set the scale parameters: shrink the x direction by a factor of .2,
     *      the y direction by a factor of .5, and the z direction by a factor of .5
     */
    tr->scale = glm::vec3(.2f, .5f, .5f);
    /*
     * Get the matrix for the combined transformation and send to the vertex shader.
     */
    auto tr_matrix = tr->matrix();

    glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(tr_matrix));
    
    glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);

    delete tr;
    
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
    auto* window = glfwCreateWindow(w_width, w_height, "Use Transform Object", NULL, NULL);
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
