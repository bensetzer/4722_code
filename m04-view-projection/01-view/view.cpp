
/*
 * To support the camera and projection we introduce the 'view' class.
 * This class is mainly a collection of attributes.
 * In this example we just concern ourselves with the camera and its attributes:
 *      camera_up
 *      camera_forward
 *      camera_left
 *      camera_position
 *   The first three of these attributes are vectors specifying the orientation
 *      of the camera
 *   The last attribute is a position.  These are all stored as vec3's
 *
 *   We have to be careful in changing the camera orientation since the three vectors
 *      have specified relationships:  all three are unit vectors (length 1);
 *      and, they are each perpendicular to the other two.
 */


#include "GLM/gtc/type_ptr.hpp"
#include <glad/gl.h>

#include "learnopengl/shader.h"

#include <cs4722/artifact.h>
#include "cs4722/view.h"



static GLuint program;
static std::vector<cs4722::artifact*> artifact_list;

/*
 * A view object manages the view and projection transformations to be applied
 * when rendering a scene.
 */
static cs4722::view *the_view;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;



void init()
{

    /*
     * Create a view object.
     */
    the_view = new cs4722::view();

    /*
     * Here are several groups of initializations for the camera in the_view.
     * Uncomment a group to see how the scene looks.
     * Hint: in CLion you can highlight a group of lines and type CTL-/ to toggle commenting of the lines.
     */

    /*
     * The first group is the default camera setup.
     * These assignments don't change the camera position or orientation.
     */
    /*   To uncomment the following, remove the space between * and / at the end of this line     * /
    the_view->camera_position = glm::vec3(0, 0, 0);     // at origin
    the_view->camera_up = glm::vec3(0, 1, 0);           // positive y axis
    the_view->camera_left = glm::vec3(-1, 0, 0);        // negative x axis
    the_view->camera_forward = glm::vec3(0, 0, -1);     // negative z axis
    /* */

   /*
    *  Look at scene from above
    */
    /*   To uncomment the following, remove the space between * and / at the end of this line     * /
    the_view->camera_position = glm::vec3(0, 1, 0);
    // point camera towards (0, 0, -.5).  Subtract camera position from that to get the forward vector
    // normalize to a unit vector
    the_view->camera_forward = glm::normalize(glm::vec3(0, 0, -.5) - the_view->camera_position);
    // left is still along the negative x axis
    the_view->camera_left = glm::vec3(-1, 0, 0);
    // up should be perpendicular to both forward and left
    // when wrapping the fingers of the right hand from forward to left, the thumb points up.
    the_view->camera_up = glm::cross(the_view->camera_forward,the_view->camera_left);
    /* */


    /*
     * Look at the scene from a little to the right.
     */
    /*   To uncomment the following, remove the space between * and / at the end of this line     * /
    the_view->camera_position = glm::vec3(.2, 0, 0);
    // point camera towards (0, 0, -1).  Subtract camera position from that to get the forward vector
    // normalize to a unit vector
    the_view->camera_forward = glm::normalize(glm::vec3(0, 0, -1) - the_view->camera_position);
    // up is still along the positive y axis
    the_view->camera_up = glm::vec3(0,1,0);
    // left should be perpendicular to both up and forward.
    // when wrapping the fingers of the right hand from up to forward, the thumb points left.
    the_view->camera_left = glm::cross(the_view->camera_up, the_view->camera_forward);
    /* */


    Shader shader("vertex_shader.glsl","fragment_shader.glsl");
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);


    // create a shape
    //cs4722::shape *b =  new cs4722::cylinder(.75, 20);
//    cs4722::shape* b = new cs4722::sphere(15, 50);
    cs4722::shape* b = new cs4722::block();

    auto* artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(.8, 0, 0);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artifact_list.push_back(artf);

    artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(-.8, 0, 0);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artifact_list.push_back(artf);


    artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(-.4, 0, -.4);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artifact_list.push_back(artf);


    artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(.4, 0, -.4);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artifact_list.push_back(artf);


    artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(0, 0, -.8);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artifact_list.push_back(artf);



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

void render()
{


    static auto last_time = 0.0;
    auto time = glfwGetTime();
    auto delta_time = time - last_time;
    last_time = time;


    /*
    * The view transform is a combination or rotations and translations that bring the camera
    *      to the origin and orient it in the standard way: positive x goes to the right;
    *      positive z goes away from us; positive y goes up.
    *
    *  glm provides the 'lookAt' function to compute a view transformation.
    *  The first parameter is the position of the camera
    *  The second parameter is a point that is directly in front of the camera
    *      An easy way to compute such a point is to simply add the forward vector to the position
    *  The third parameter is a vector pointing in the direction of the camera up
    *
    *  The view transform is computed outside the artifact loop because the view applies
    *      to all of the artifacts in the same way.
    */
    glm::mat4x4 view_transform = glm::lookAt(the_view->camera_position,
                                             the_view->camera_position + the_view->camera_forward,
                                             the_view->camera_up);



    for (auto artf : artifact_list) {
        artf->animate(time, delta_time);

        auto model_matrix = artf->animation_transform.matrix() * artf->world_transform.matrix();

        auto transform = view_transform *  model_matrix;
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(transform));
        glDrawArrays(GL_TRIANGLES,  artf->the_shape->buffer_start, artf->the_shape->buffer_size);
    }
}


int
main(int argc, char** argv)
{
    glfwInit();

    // set the OpenGL version to use: 4.3
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Animation with Camera", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);
//    cs4722::setup_debug_callbacks();
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
