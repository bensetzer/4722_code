/*
 * Does not use intermediary storage c_buf and p_buf for buffer storage
 */
/*
 * We will create scene by creating and rendering multiple artifacts.
 * There are a few pointers to keep in mind about how this will be done.
 *
 *      Artifacts will be created in the init function
 *      Each artifact will be assigned a shape
 *      Two artifacts can share the same shape
 *          The two artifacts can, and usually will, apply different transforms to the shape
 *      We will create a list of artifacts
 *          The buffer setup will iterate through the list to get to the necessary shape data
 *          Each artifact will be rendered separately since the transform will generally be different
 *              for different artifacts
 *
 *  There are some specific concerns when dealing with the shapes
 *  All the shape positions will go into the same buffer.  Ditto for colors and other vertex attributes.
 *  We will keep track of where in the buffer each shape starts (counting vertices)
 *  We will also keep track how many vertices are in each shape
 *      These are attributes of the shape and will be set when setting up the buffers
 *  Only one shape will be drawn for an artifact, the shape that is assigned to it.
 *      We will use the information about where the shape starts in the buffer and how many vertices
 *      it has to set the second and third parameters of the glDrawArrays function
 *
 */


#include "GLM/gtc/type_ptr.hpp"

#include <vector>
//#include <numeric>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/x11.h"
#include "cs4722/shape.h"
#include "cs4722/artifact.h"


static GLuint program;

/*
 * A list of pointers to artifact.
 * The list itself is not a pointer!  Watch where .  and -> are used.
 *      Good editors will help you with . vs ->
 */
static std::vector<cs4722::artifact*> artifact_list;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;

void init()
{


    Shader shader("vertex_shader-m02.glsl" ,"fragment_shader-m02.glsl" );
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);


    /*
     * Create some shapes.
     * We normally would create just the shapes we need.
     * However, for this example we want a variety of shapes you can try out with the artifacts defined below.
     */
    cs4722::shape* cy1 =  new cs4722::cylinder(.05, 20);
    cs4722::shape* cy2 = new cs4722::cylinder();
    cs4722::shape* sp1 = new cs4722::sphere(15,50);
    cs4722::shape* sp2 = new cs4722::sphere(4, 5);
    cs4722::shape* bl  = new cs4722::block();
    cs4722::shape* tr  = new cs4722::torus();

    /*
     * Create an artifact and configure it
     */
    auto* artf = new cs4722::artifact();
    /*
     * Pick a shape and assign it to the artifact.
     * Try changing the shapes used.
     */
    artf->the_shape = sp1;
    artf->world_transform.translate = (glm::vec3(.5, -.5, 0));
    artf->world_transform.rotation_axis = (glm::vec3(0, 1, 1));
    artf->world_transform.rotation_angle = (M_PI / 3);
    artf->world_transform.scale = (glm::vec3(.4f, .6f, .4f));
    /*
     * Add the artifact to the list.
     */
    artifact_list.push_back(artf);


    auto* artf1 = new cs4722::artifact();
    /*
     * This is a different artifact, but it uses the same shape as the first artifact (unless
     *      you made a change).
     *  This works just fine.
     *  As distributed, this example will display two spheres, somewhat elongated, at the bottom
     *      corners of the window.
     */
    artf1->the_shape = sp1;
    artf1->world_transform.translate = (glm::vec3(-.5, -.5, 0));
    artf1->world_transform.rotation_axis = (glm::vec3(1, 1, 0));
    artf1->world_transform.rotation_angle = (M_PI / 4);
    artf1->world_transform.scale = (glm::vec3(.4f, .6f, .4f));
    artifact_list.push_back(artf1);

    auto* artf2 = new cs4722::artifact();
    artf2->the_shape = tr;
    artf2->world_transform.translate = (glm::vec3(-.5, .5, 0));
    artf2->world_transform.rotation_axis = (glm::vec3(1, 1, 0));
    artf2->world_transform.rotation_angle = (M_PI / 4);
    artf2->world_transform.scale = (glm::vec3(.4f, .6f, .4f));
    artifact_list.push_back(artf2);

    /*
     *  Uncomment the following lines to add another artifact to the scene.
     *  All you have to do is create an artifact, assign a shape, configure other attributes
     *      and add the artifact to the list.
     *  The artifact will then will be part of the scene.
     */
//    auto* artf3 = new cs4722::artifact();
//    artf3->the_shape = bl;
//    artf3->world_transform.translate = (glm::vec3(.5, .5, 0));
//    artf3->world_transform.rotation_axis = (glm::vec3(1, 0, 1));
//    artf3->world_transform.rotation_angle = (M_PI / 4);
//    artf3->world_transform.scale = (glm::vec3(.4f, .4f, .2f));
//    artifact_list.push_back(artf3);



    /*
     * To allocate buffer space we need to figure out how many total vertices we are working
     * with.
     * total_vertex_count will receive that value.
     */
    auto total_vertex_count = 0;
    for(auto *artf : artifact_list) {
        /*
         * Set two attributes of the shape.
         * These tell where the associated shape data is in the buffer using
         * the start position (in terms of vertices) and the number of vertices.
         *
         * total_vertex_count is the next index, counting vertices, available.
         * This is much like the code for a stack.
         */
        artf->the_shape->buffer_start = total_vertex_count;
        /*
         * For convenience, the attribute buffer_size is set to the number of vertices.
         * It would be just as easy to call get_size(), but that's for a future change.
         */
        artf->the_shape->buffer_size = artf->the_shape->get_size();
        /*
         * Observation: if two or more artifacts are sharing a shape, this code will put the
         * shape data into the buffer multiple times.
         * As it turns out, the last copy of that data will be used for all the artifacts that share that shape.
         *      This means we are wasting the space taken by the first copy (or copies).
         * In the example 05-shared-shapes we will discuss how to be more efficient and include a shape's data
         *      only once in the buffers.
         */

        /*
         * Update the total vertex count
         */
        total_vertex_count += artf->the_shape->get_size();

    }



    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    /*
     *  Set up buffers
     */
    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);


    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    /*
     * We are allocating storage here but not providing data.
     */
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, nullptr,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT,
                          GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * total_vertex_count, nullptr,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(b_color);




    /*
    *   We now put the artifact shape data into the buffers.
     *   The code will have to use the buffer_start attribute of each shape to
     *   figure out where to place the data into the buffer.
   */
    for (auto artf : artifact_list) {

        /*
         * Now put data into the buffers
         */

        /*
         * Get the position data for the shape associated with this artifact.
         */
        auto pos = artf->the_shape->positions();
        /*
         * Bind the buffer we are going to use, the positions buffer in this case.
         */
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        /*
         * Put the data into the buffer.
         *
         * The first parameter is the binding point.
         *
         * The second paramter is the starting point for the data measured in bytes.
         *
         * The third parameter is the number of bytes of data.
         *
         * The last parameter is the data itself.
         *      pos is a std::vector object, the data() function returns a pointer to the
         *      actual data.
         */
        glBufferSubData(GL_ARRAY_BUFFER,
                             4 * 4 * artf->the_shape->buffer_start,
                             4 * 4 * artf->the_shape->buffer_size,
                             pos->data());
        // pos is dynamically allocated, we must free it.
        delete pos;


        /*
         * Now the colors
         */
        auto colors = artf->the_shape->colors();
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferSubData(GL_ARRAY_BUFFER,
                        4 * 1 * artf->the_shape->buffer_start,
                        4 * 1 * artf->the_shape->buffer_size,
                        colors->data());
        delete colors;


    }



}

//----------------------------------------------------------------------------
//
// display
//

void
render()
{
    /*
     * Render each artifact in turn.
     *
     * artf will iterate over the different artifacts in the list.
     */
    for (auto artf : artifact_list) {
        /*
         *  Get the world transform from the current artifact and set the transform variable in the vertex
         *      shader.
         *  NOTE: we are using the same vertex shader as in the previous examples.
         *      The vertex shader doesn't care where we get the information we give it as long as it makes sense.
         */
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(artf->world_transform.matrix()));
        /*
         *  The second parameter to glDrawArrays specifies where in the buffer to start finding the data
         *      for the shape of this artifact.
         *      This is counted in vertices so applies to both colors and positions.
         *   The third parameter specifies how many vertices are used to describe this shape.
         */
        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start, artf->the_shape->buffer_size);
    }
}

int
main(int argc, char** argv)
{
    glfwInit();

    // set the OpenGL version to use: 3.3
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Shapes", NULL, NULL);
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
