

/*
 * A view object contains attributes to describe a projection in addition to the
 *      camera attributes we saw in the previous example.
 *  The projection transform attributes are those needed for the infinitePerspective() function
 *      described in the notes:
 *      * Field of view in the y direction
 *      * Aspect ratio, width divided by height
 *      * Distance to the near face of the view frustum
 *
 *
 *
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
     * Change the z position to make the scene visible
     */
    the_view->camera_position = glm::vec3(0, 0, 1);

    /*
     *  Look at scene from above
     */
    // place above the scene
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
     * Move back to z = 1 as well to get a better view of the scene
     */
    /*   To uncomment the following, remove the space between * and / at the end of this line     * /
    // place above the scene
    the_view->camera_position = glm::vec3(.2, 0, 1);
    // point camera towards (0, 0, -1).  Subtract camera position from that to get the forward vector
    // normalize to a unit vector
    the_view->camera_forward = glm::normalize(glm::vec3(0, 0, -1) - the_view->camera_position);
    // up is still along the positive y axis
    the_view->camera_up = glm::vec3(0,1,0);
    // left should be perpendicular to both up and forward.
    // when wrapping the fingers of the right hand from up to forward, the thumb points left.
    the_view->camera_left = glm::cross(the_view->camera_up, the_view->camera_forward);
    /* */

    /*
     * Default perspective parameters
     */
//    the_view->perspective_near = .1f;
//    the_view->perspective_fovy = M_PI / 2;
//    the_view->perspective_aspect = 3;

    /*
     * Change the fov
     */
    the_view->perspective_fovy = .5 * M_PI;


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

    /*
       * Compute the projection transformation using the view attributes.
       */
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                 the_view->perspective_aspect, the_view->perspective_near);
    /*
     * The view-projection transformation is the product.
     * Notice that the view transform is applied first, then the projection transform.
     */
    auto vp_transform = projection_transform * view_transform;

    for (auto artf : artifact_list) {
        artf->animate(time, delta_time);

        auto model_matrix = artf->animation_transform.matrix() * artf->world_transform.matrix();

        auto transform = vp_transform *  model_matrix;
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

    /*
     * We can change the aspect ratio of the window, provided we also change the perspective
     * projection aspect ratio to match.
     */
    auto aspect_ratio = 1.0f;
    float ratio = 0.9f;             // ratio of window size to screen size
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
    //std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
    int w_width = glm::min(ratio * width, aspect_ratio * ratio * height);
    int w_height = w_width / aspect_ratio;
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "User Interaction", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    init();
    /*
     * Set the perspective projection aspect ratio to match the window.
     */
    the_view->perspective_aspect = aspect_ratio;


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
