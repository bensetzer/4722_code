
/*
 * In this example we introduce the key class that we will use to build models in most of our examples, the
 *      class named artifact.
 *  The artifact class pulls together a number of attributes that we can use to represent the characteristics
 *      of a single object in a scene:
 *      *  A shape that describes the basic geometric shape of the object  (attribute the_shape)
 *      * A transform object that describes how to size, orient, and place the object in the scene
 *              (attribute world_transform)
 *      * Another transform object that describes how to change the size, orientation, and placement of
 *          an object in the scene. (attribute animate_transform)
 *      * Several other attributes will come into use in later modules and will be explained there.
 *
 *  In a typical example we will create a number of artifacts in the init function.
 *  Each artifact will be assigned an appropriate shape.
 *  Each the world_transform for each artifact will be set to give the initial size, orientation, and
 *      placement of the artifact
 */

#include "GLM/gtc/type_ptr.hpp"

#include <algorithm>


#include <glad/gl.h>
#include <GLFW/glfw3.h>


#include "learnopengl/shader.h"

#include "cs4722/x11.h"
#include "cs4722/artifact.h"


static GLuint program;
// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;


/*
 * The artifact object must be global since the artifact is set up in one function (init) and
 *      used in another (display).
 *  Note: this is C++, so artf is an actual object, not a pointer to an object.
 */
static cs4722::artifact artf;

static int number_of_vertices;

void init(void)
{

    Shader shader("vertex_shader-m02.glsl" ,"fragment_shader-m02.glsl" );
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);


    /*
     * Create a shape.
     * Uncomment a different line to change the shape.
     */
//    cs4722::shape *b =  new cs4722::cylinder(.05, 20);
//    cs4722::shape* b = new cs4722::cylinder();
//    cs4722::shape* b = new cs4722::sphere(15,50);
//    cs4722::shape* b = new cs4722::sphere(4, 5);
//    cs4722::shape* b = new cs4722::block();
    cs4722::shape* b = new cs4722::torus();

    /*
     * artf is defined at the file level so it can be shared with the display function.
     */
    /*
     * Set the shape of the artifact.
     */
    artf.the_shape = b;
    /*
     * Set the various parameters of the world_transformation.
     * The order in which they are set does not matter.
     * You can try to change the order and see that it won't make a difference in the display.
     */
    artf.world_transform.translate = glm::vec3(.5, -.5, 0);
    artf.world_transform.rotation_angle = -M_PI / 4;
    artf.world_transform.scale = glm::vec3(.2, .5, .5);
    artf.world_transform.rotation_axis = glm::vec3(0,0,1);

    /*
     * The shape provides the vertices, so we 'ask' the shape for the number of those vertices.
     */
    number_of_vertices = artf.the_shape->get_size();

    /*
     * Go to the shape belonging to the artifact to get positions and colors
     */
    auto* positions = artf.the_shape->positions();
    auto* colors = artf.the_shape->colors();

    /*
     * The rest of init proceeds as before.
     */

    // Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);


    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//    glBufferStorage(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, positions->data(), 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, positions->data(),  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT,
        GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
//    glBufferStorage(GL_ARRAY_BUFFER, 1 * 4 * number_of_vertices, colors->data(), 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * number_of_vertices, colors->data(),  GL_STATIC_DRAW);
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(b_color);

    delete positions;
    delete colors;
    

}

void render()
{
 
 
    glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
    glClear(GL_DEPTH_BUFFER_BIT);
    /*
     * Get the world_transform matrix from the artifact.
     */
    auto tr = artf.world_transform.matrix();

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
    auto* window = glfwCreateWindow(w_width, w_height, "Use Artifact", NULL, NULL);
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
