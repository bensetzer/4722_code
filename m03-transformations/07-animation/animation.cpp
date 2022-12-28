
/*
 * We will implement animation in our examples by changing the animation_transform in artifacts.
 * If we make these changes periodically, sufficiently frequently,  we can simulate smooth motion.
 *
 * Recall that the display function is called repeatedly by the event loop.
 * Each time the display function executes we can modify the animation_transform for artifacts and
 *      redisplay them using the new values.
 *
 *  We will use the animate method in class artifact to carry out animation.
 *  In the class artifact itself, the method does nothing.
 *  It is intended that subclasses implement the animate method in order to provide different
 *      kinds of animation.
 *
 *  Tha animate method requires two parameters:  the time and the delta time.
 *  The time value should be the number of seconds since some fixed point in time to the time
 *      when the method is called.
 *      Using GLFW, the time will be the number of seconds since the program started.
 *  The delta time is the number of seconds since the last time the display function was called.
 *  Physics based animation often use the delta time.
 *
 *  In most of our examples we will use the class artifact_rotating for animated artifacts.
 *  This class overrides the animation method to set the angle of rotation for the animation_transform
 *      to be the time multiplied by a fixed value set when the artifact is created.
 *      The fixed value is the rotation rate in radians per second.
 *  The rotation axis and the rotation center of the animation_transform can be set when
 *      creating the artifact in order to define the rotation.
 */


#include "GLM/gtc/type_ptr.hpp"

#include <glad/gl.h>

#include "learnopengl/shader.h"

#include "cs4722/artifact.h"

static std::vector<cs4722::artifact*> artifact_list;
static GLuint program;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;


void init(void)
{
    Shader shader("vertex_shader-m02.glsl" ,"fragment_shader-m02.glsl" );
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

	
    // create a shape
    //cs4722::shape *b =  new cs4722::cylinder(.75, 20);
    cs4722::shape* b = new cs4722::sphere(15,50);
    // cs4722::shape* b = new cs4722::block();
    /*
     * Create an artifact_rotating.
     */
    auto* artf = new cs4722::artifact_rotating();
    artf->the_shape = b;
    artf->world_transform.translate = glm::vec3(.5, -.5, 0);
    artf->world_transform.scale = glm::vec3(.4f, .6f, .4f);
    /*
     * The rotation axis is tilted 45 degrees, halfway between the x and y axes.
     */
    artf->animation_transform.rotation_axis = glm::vec3(1, 1, 0);
    /*
     * The center of rotation is the origin.
     * This means that the artifact will rotate around a point not at its center.
     */
    artf->animation_transform.rotation_center = glm::vec4(0,0,0,1);
    /*
     * Notice that the rotation rate is not part of the transform, it is an attribute
     * of the artifact.
     * The attribute is defined in artifact_rotating.
     *
     * The rotational velocity used here is 60 degrees per secons.
     * A single rotation will take six seconds..
     */
    artf->rotation_rate = M_PI / 3;

    artifact_list.push_back(artf);

	
    cs4722::shape* b1 = new cs4722::block();
    auto* artf1 = new cs4722::artifact_rotating();
    artf1->the_shape = b1;
    artf1->world_transform.translate = glm::vec3(-.5, 0, 0);
    /*
     * The axis of rotation for animation is halfway between the x and z axes.
     */
    artf1->animation_transform.rotation_axis = glm::vec3(1, 0, 1);
    artf1->animation_transform.rotation_center = glm::vec4(0,0,0,1);
    artf1->rotation_rate = M_PI / 3;
//    obj1->animation_transform.rotation_center = glm::vec3(0, .3f, 0);
    artf1->world_transform.scale = glm::vec3(.2f, .6f, .2f);

    artifact_list.push_back(artf1);

    /*
     *  Set up buffers
     */
    auto total_vertex_count = 0;
    for(auto *artf : artifact_list) {
        if(artf->the_shape->buffer_size == 0) {
            artf->the_shape->buffer_start = total_vertex_count;
            artf->the_shape->buffer_size = artf->the_shape->get_size();
            total_vertex_count += artf->the_shape->get_size();
        } else {
            artf->shape_is_shared = true;
        }
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(b_color);

    for (auto artf : artifact_list) {
        if(!artf->shape_is_shared) {
            auto pos = artf->the_shape->positions();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            pos->data());
            delete pos;

            auto colors = artf->the_shape->colors();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 1 * artf->the_shape->buffer_start,
                            4 * 1 * artf->the_shape->buffer_size,
                            colors->data());
            delete colors;
        }
    }


}

//----------------------------------------------------------------------------
//
// display
//


void
display()
{

    /*
     * To compute the delta time we have to keep track of the last time that dispaly was called.
     * A 'static' variable defined inside a method is initialized when the program starts but
     * after that retains any new value it is assigned between function calls.
     * In most languages we would have to define last_time outside of the function, but C++ and C
     * allow this approach.
     */
    static auto last_time = 0.0;
//    printf("last time %.5f\n", last_time);
    /*
     * Call  the GLFW function to get the number of seconds since the program started.
     */
    auto time = glfwGetTime();
    /*
     * Compute the delta time: the current time (time) minus that last time the display
     * function was called.
     */
    auto delta_time = time - last_time;
    /*
     * Update last_time.
     * The value will be retained even while the function is not executing.
     */
    last_time = time;

    // uncomment the following few lines to see the frame rate computed two different ways
    static double  arate = 0.0;
//    auto weight_average = 10.0;   // displayed rate is volatile reflecting the current frame
    auto weight_average = 100.0;  // displayed rate is more stable but takes longer to stabilize
    auto x = (weight_average * arate + 1 / delta_time) / (weight_average+1);
    arate  = x;
    printf("average frame rate is %f   instantaneous frame rate %f\n", arate, 1 / delta_time);




    for (auto artf : artifact_list) {
        /*
         * For each artifact, call animate to update attributes of the artifact.
         */
        artf->animate(time, delta_time);
        /*
         * Then render the artifact.
         */
        auto model_matrix = artf->animation_transform.matrix()
                            * artf->world_transform.matrix();
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(model_matrix));
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Animation", NULL, NULL);
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
