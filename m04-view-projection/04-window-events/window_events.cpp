
/*
 * Window events are events caused by windows.
 *  These events report changes to the associated window.
 *  For example, if the user drags the edges of a window to change the window size, an event would be generated.
 *  By reacting to a window changing size, we can scale the rendering of the scene to the new window.
 *
 *  This example also shows how to take an application to full screen and back.
 *      The F11 key will be used to toggle between full screen and back.
 *
 *  Finally, the escape key will be 'wired' to terminate the application.
 *
 *  Note: the window for this example starts out very small in order to make it very clear what
 *      changing the window size is doing.
 */

#include "GLM/gtc/type_ptr.hpp"
#include <glad/gl.h>

#include "learnopengl/shader.h"
#include "cs4722/artifact.h"
#include "cs4722/view.h"

static cs4722::view *the_view;
static GLuint program;
static std::vector<cs4722::artifact*> artf_list;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;


void init()
{

    the_view = new cs4722::view();
//    the_view->camera_position = glm::vec3(0, 0, 3);

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
    artf_list.push_back(artf);

    artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(-.8, 0, 0);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artf_list.push_back(artf);


    artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(-.4, 0, -.4);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artf_list.push_back(artf);


    artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(.4, 0, -.4);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artf_list.push_back(artf);


    artf = new cs4722::artifact();
    artf->the_shape = (b);
    artf->world_transform.translate = glm::vec3(0, 0, -.8);
    artf->world_transform.scale = (glm::vec3(.3, .3, .3));
    artf_list.push_back(artf);



    /*
     *  Set up buffers
     */
    auto total_vertex_count = 0;
    for(auto *artf : artf_list) {
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

    for (auto artf : artf_list) {
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
    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect, the_view->perspective_near);
    auto vp_transform = projection_transform * view_transform;


    static auto last_time = 0.0;
    auto time = glfwGetTime();
    auto delta_time = time - last_time;
    last_time = time;


    for (auto artf : artf_list) {

        artf->animate(time, delta_time);

        auto model_transform = artf->animation_transform.matrix() * artf->world_transform.matrix();
        auto transform = vp_transform * model_transform;
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(transform));

        
        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start, artf->the_shape->buffer_size);
       

    }
}


float lr_scale = .1f;
float fb_scale = .1f;
float ud_scale = .1f;

float lr_pan_scale = .01f;
float ud_pan_scale = lr_pan_scale;


/*
 * These variables are used to hold the window height and width and position to use when
 *  exiting full screen.
 */
static int old_width, old_height, old_xpos, old_ypos;

void general_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
    // the_scene->the_view->key_callback(window, key, scancode, action, mods);
    // std::cout << "key " << key << std::endl;
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
    {
        // only process key presses and repeats
    }
    else if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
    {
        view->camera_position += lr_scale * view->camera_left;
    }
    else if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
    {
        view->camera_position -= lr_scale * view->camera_left;
    }
    else if (key == GLFW_KEY_W)
    {
        view->camera_position += fb_scale * view->camera_forward;
    }
    else if (key == GLFW_KEY_S)
    {
        view->camera_position -= fb_scale * view->camera_forward;
    }
    else if (key == GLFW_KEY_UP)
    {
        view->camera_position += ud_scale * view->camera_up;
    }
    else if (key == GLFW_KEY_DOWN)
    {
        view->camera_position -= ud_scale * view->camera_up;
    }
    else if (key == GLFW_KEY_ESCAPE)
    {
        /*
         * If the escape key is pressed, then the following function will set the GLFW
         * flag variable 'windowShouldClose' to true.
         * Then the event loop, in function main, and the program will terminate.
         * This is the proper way to terminate the program since it gives GLFW an opportunity to
         *      clean up the resources it is using.
         */
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
    {
        /*
         * If the F11 key is pressed, but not a repeat generated by a continued press,
         * then toggle between full screen and windowed.
         */

        /*
         * If the monitor attribute of the window is set, not NULL, then
         *  the window is currently in full screen according to GLFW conventions.
         *
         */
        if (!glfwGetWindowMonitor(window)) {
            /*
             * This code is executed if the monitor attribute of the window is NULL.
             * That means that the window is currently not full screen.
             * So, execute code to take it to full screen.
             */

            /*
             * Save the window position and size.
             * The variables used to save these values are defined just above the callback function header.
             * A pointer to each variable is passed (&old_xpos is the address in memory of the variable old_xpos).
             * In this way the function can change the values of those variables.
             */
            glfwGetWindowPos(window, &old_xpos, &old_ypos);
            glfwGetWindowSize(window, &old_width, &old_height);
            /*
             * Get an object representing the primary monitor.
             *
             * While it would be nicer to figure out which monitor is being used on a multiple monitor setup,
             *      that doesn't add much to the example.
             */
            auto monitor = glfwGetPrimaryMonitor();
            /*
             * The mode of a monitor in GLFW is an object that contains attributes of the monitor, such as
             * its resolution.
             */
            const auto* mode = glfwGetVideoMode(monitor);
            /*
             * Now switch to full-screen.
             * The second parameter is a monitor object, not NULL, so that indicates full-screen.
             * It isn't necessary to use the resolution of the monitor for width and height.
             *      However, using other values will force a change of resolution
             */
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            /*
             * This code is reached if F11 was pressed while the application is in full-screen mode.
             * Using nullptr as the second argument signals that the application should go into windowed mode.
             * The size and position of the window should be the ones stored when going into full-screen,
             *      so the old window will be restored.
             */
            glfwSetWindowMonitor(window, nullptr, old_xpos, old_ypos, old_width, old_height, 0);

        }
    }
    else
    {

    }
}

/*
 * This is a callback registered in function main.
 * This function is called when the size of the window changes.
 */
void window_size_callback(GLFWwindow* window, int width, int height)
{
    /*
     * grab the view since we will be changing the perspective aspect.
     */
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
    /*
     * Find out the width and height of the framebuffer for the window.
     * This is evidently the best way to find out how big the window is.
     *      The framebuffer is sized to match the window size.
     * Notice that the addresses of the width and height variables are passed to the
     *      glfwGetFramebufferSize function.
     */
    int widthfb, heightfb;
    glfwGetFramebufferSize(window, &widthfb, &heightfb);
    /*
     * The viewport is the amount of the window to use to display the rendered scene.
     * The parameters used here to set the viewport cause the viewport to cover the whole window.
     */
    glViewport(0, 0, widthfb, heightfb);
    /*
     * Change the perspective aspect.
     * Otherwise, the scene will be distorted if the aspect ratio is changed.
     */
    view->perspective_aspect =
        (static_cast<float>(widthfb) / static_cast<float>(heightfb));

}

// used in mouse motion
static double last_x = 0.0;
static double last_y = 0.0;

void move_callback(GLFWwindow* window, double xpos, double ypos)
{
    static auto last_x = 0.0f;
    static auto last_y = 0.0f;
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
     auto dx = xpos - last_x;
     auto dy = ypos - last_y;
     last_x = xpos;
     last_y = ypos;


    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        // std::cout << "(" << xpos << "," << ypos << ")" << std::endl;
        // auto const rot_lr = glm::mat3(glm::rotate(glm::identity<glm::mat4>(),
        //     static_cast<float>(-dx * lr_pan_scale), camera_up_));
        auto const rot_lr = glm::mat3(glm::rotate(glm::identity<glm::mat4>(),
            static_cast<float>(-dx * lr_pan_scale), glm::vec3(0, 1, 0)));
        view->camera_left = rot_lr * view->camera_left;
        view->camera_forward = rot_lr * view->camera_forward;
        view->camera_up = rot_lr * view->camera_up;

        auto const rot_ud = glm::mat3(glm::rotate(glm::identity<glm::mat4>(),
            static_cast<float>(dy * ud_pan_scale), view->camera_left));
        view->camera_up = rot_ud * view->camera_up;
        view->camera_forward = rot_ud * view->camera_forward;
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
//    cs4722::setup_debug_callbacks();

    init();
    the_view->perspective_aspect = aspect_ratio;

    glfwSetWindowUserPointer(window, the_view);
    glfwSetKeyCallback(window, general_key_callback);
    glfwSetCursorPosCallback(window, move_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

	
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
