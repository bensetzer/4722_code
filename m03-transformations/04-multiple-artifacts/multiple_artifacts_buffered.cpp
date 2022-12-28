
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

#include <list>

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
static std::list<cs4722::artifact*> artifact_list;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;



void init(void)
{


    Shader shader("vertex_shader-m04.glsl" ,"fragment_shader-m04.glsl" );
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
     * The next step is to combine the data form the different shapes into single lists.
     * It is then easy to put the data into the buffers.
     *
     * It would also work to put the data directly from the shapes into the buffers.
     * This would use a different pattern of function calls.
     * That approach would probably be more efficient, but the current code works fine
     *      for our purposes.
     *
     *  So, we create vectors to collect the data.
     */
    auto* p_buf = new std::vector<glm::vec4>();
    auto* c_buf = new std::vector<cs4722::color>();

    /*
     * We iterate over the artifact list, getting each artifact.
     */
    for (auto current_artf : artifact_list) {

        // it's easier to use 'current_shape' then 'current_artf->the_shape' repeatedly.
        auto* current_shape = current_artf->the_shape;

        /*
         * Set two attributes of the shape.
         * To understand these, note that the data for this shape will go into p_buf and c_buf
         *      following the data already placed there.
         *  So, the index at which this shape will start is the size of the lists before putting int
         *      the new shape data.
         *  The same value works for both buffers since the count is by vertices not bytes.
         */
        current_shape->buffer_start = p_buf->size();
        /*
         * For convenience, the attribute buffer_size is set to the number of vertices.
         * It would be just as easy to call get_size(), but that's for a future change.
         */
        current_shape->buffer_size = current_shape->get_size();
        /*
         * Observation: if two or more artifacts are sharing a shape, this code will but the
         * shape data into the buffer multiple times.
         * As it turns out, the last copy of that data will be used for all the artifacts that share that shape.
         *      This means we are wasting the space taken by the first copy (or copies).
         * In the example 05-shared-shapes we will discuss how to be more efficient and include a shape's data
         *      only once in the buffers.
         */




        /*
         * Now put the position data into p_buf.
         * First get a pointer to the vector of positions.
         */
        auto* vertices = current_shape->positions();
        /*
         * Then copy the data into p_buf after the data that is already in p_buf.
         *
         * The first parameter indicates where to start placing the data in p_buf: at the end
         *
         * The second parameter specifies where to start taking data from vertices: at the beginning
         *
         * The third parameter specifies where to stop taking data from vertices: at the end.
         */
        p_buf->insert(p_buf->end(), vertices->begin(), vertices->end());
        // we don't need vertices any more, the data is in p_buf.
        delete vertices;

        /*
         * the colors are handled similarly.
         */
        auto* colors = current_shape->colors();
        c_buf->insert(c_buf->end(), colors->begin(), colors->end());
        delete colors;

    }
   

    /*
     * Now fill the buffers.
     * The code is similar to that in earlier examples.
     * We're using p_buf->size() and c_buf->size() instead of number_of_vertices
     *      The two values are actually the same (or should be ;)
     *  The data is in p_buf
     */


    // Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

   
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//    glBufferStorage(GL_ARRAY_BUFFER, 4 * 4 * p_buf->size(), p_buf->data(), 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * p_buf->size(), p_buf->data(),  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT,
        GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
//    glBufferStorage(GL_ARRAY_BUFFER, 1 * 4 * c_buf->size(), c_buf->data(), 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * c_buf->size(), c_buf->data(),  GL_STATIC_DRAW);
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(b_color);

    // p_buf and c_buf are no longer needed.
    delete p_buf;
    delete c_buf;


}

//----------------------------------------------------------------------------
//
// display
//

void
display()
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
    float ratio = .7f;
    int w_width = int(ratio * size);
    int w_height = w_width;
    int w_x = int(size * (1 - ratio) / 2);
    int w_y = w_x;
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Multiple Artifacts", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    init();

    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
        glClear(GL_DEPTH_BUFFER_BIT);
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
