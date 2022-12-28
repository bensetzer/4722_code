
/*
 * This program introduces user interaction using callback functions.
 * 
 * We implement linear motion using the keyboard and camera orientation using the mouse.
 *      The left mouse button must be held down while moving the mouse in order for the motion
 *      to have an effect.
 *
 *  It is important to remember that what we are calling the event loop, in function main, repeatedly calls
 *      the display function to render the scene over and over.
 *  The function glfwPollEvents is also called in this loop.  That function triggers the callbacks, if
 *      events have happened.
 *      So, the camera state is changed in response to user interaction and then the scene is rendered again
 *      using the new values for the camera.
 */

#include "GLM/gtc/type_ptr.hpp"
//#include "GLM/glm.hpp"
#include <glad/gl.h>

#include "learnopengl/shader.h"
#include "cs4722/view.h"
#include "cs4722/artifact.h"


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

//    the_view->camera_position = glm::vec3(0, 0, 0);

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



/*
 *
 * This section contains constants that govern how fast the camera moves and rotates.
 * Camera speed would be, in a scene using first-person perspective, the speed of the character.
 * The speed of camera rotation would be mouse sensitivity.
 *
 */
/*
 * How far to move the camera in the specified direction on each click of the appropriate key.
 */
float movement_speed = .5f;
float lr_scale = movement_speed;
float fb_scale = movement_speed;
float ud_scale = movement_speed;

/*
 * How far to rotate the camera, in radians, for each unit of motion of the mouse.
 */
float lr_pan_scale = .01f;
float ud_pan_scale = lr_pan_scale;


/*
 * The following function will be called by the system whenever there is a key pressed or released.
 *  For this to happen, the function must be 'registered' with GLFW.  This happens in the main function.
 *      There is a comment at that statement.
 *
 *  The programmer using callbacks implements a function.
 *      The implementation can make use of the parameter values passed to the function.
 *      The programmer will not write code that calls this function, at least in usual circumstances.
 *
 *  See the GLFW documentation for an explanation of all the parameters.
 *  Here are the ones that we use:
 *      The first parameter is the window the had the focus when the key was pressed.
 *      The second parameter indicates which key was pressed.  This is not necessarily a Unicode code for a character
 *      The fourth parameter indicates what kind of action caused the function to be called
 *
 *  We are not really interested in the window displaying the scene but in the view object that contains the
 *      camera attributes.
 *      GLFW has a feature that allows storing a pointer to an object in a window.
 *      In the main function the view object is stored in the window object in that way
 *      In this function, the view object is fetched so that the camera attributes can be modified
 *
 *  We are doing this, rather than accessing the view object directly, because this code is destined to be
 *      moved to a different file and placed in the library.
 *      The view object would not be directly accessible in that case.
 *
 *  The main actions indicated by the fourth parameter are pressing a key, releasing a key,
 *      and a key repeating.
 *
 *  The 'key' parameter is a code that should be compared to symbolic constants defined in GLFW
 *      See this page for the full list of symbols: https://www.glfw.org/docs/3.3/group__keys.html
 *      UP and DOWN are the arrow keys
 *
 */
void general_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /*
     * Get the view object stored in the window.
     * The declared type of the value is a generic pointer so this must be cast to the proper type.
     */
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
    // std::cout << "key " << key << std::endl;  // debugging
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
    {
        /*
         * As my younger son would say, this is a bit hackish.
         * As the first test in a long series of tests, if this one passes, this function does
         * nothing at all.
         * This test passes if the event is neither a key press or a repeated key.
         * The effect is that only key presses and repeated keys can result in real results.
         */
    }
    else if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
    {
        /*
         * Key A is for moving left, so add a multiple of the camera_left vector to the position.
         */
        view->camera_position += lr_scale * view->camera_left;
    }
    else if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
    {
        /*
         * Key D is for moving right, so subtract a multiple of the camera_left vector to the position.
         */
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
    else if (key == GLFW_KEY_UP )
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
    } else
    {

    }
}


/*
 * This callback function is called when the mouse moves.
 *
 *  The first parameter is the window over which the mouse moved
 *  The second and third parameters give the position of the mouse in the window.
 *      We are not interested in the position of the mouse but only in how much the mouse has moved
 *          since the last callback.
 *          Code to compute this 'delta' is in the function code.
 */
void move_callback(GLFWwindow* window, double xpos, double ypos)
{
    /*
     * We have to remember the last position of the mouse.
     * A 'static' local variable is initialized when the program starts and then retains its value
     *      between calls.
     * The disadvantage is that this code is tied to one window at a time.
     */
    static double last_x = 0.0;
    static double last_y = 0.0;

    /*
     * Get the view object from the window.
     */
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
    // the_scene->the_view->cursor_move_callback(window, xpos, ypos);
    /*
     * The view object stores the last known position of the mouse.
     * So, we just subtract from the current position to get the distance the mouse has moved.
     */
    auto dx = xpos - last_x;
    auto dy = ypos - last_y;
    /*
     * Update the last known position for the next mouse event.
     */
    last_x = xpos;
    last_y = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        /*
         * Panning the camera is implemented by rotating the camera left and right.
         *  This does not change the position of the camera but changes its orientation.
         *
         *  An important question is which axis to rotate about?
         *  There are two good choices: the up vector of the camera and the positive y-axis.
         *  The code below ues the y-axis. However, you can change that by uncommenting the first
         *      definition of rot_lr.
         *
         *  rot_lr is a rotation matrix.
         *  The function glm::rotate returns a mat4 which must be converted to a mat3 in order to
         *      transform the camera vectors.
         *  The amount of rotation is a multiple of the amount the mouse has moved in the x direction.
         */
        // std::cout << "(" << xpos << "," << ypos << ")" << std::endl;
        // auto const rot_lr = glm::mat3(glm::rotate(glm::identity<glm::mat4>(),
        //     static_cast<float>(-dx * lr_pan_scale), camera_up_));
        auto const rot_lr = glm::mat3(glm::rotate(glm::identity<glm::mat4>(),
            static_cast<float>(-dx * lr_pan_scale), glm::vec3(0, 1, 0)));
        view->camera_left = rot_lr * view->camera_left;
        view->camera_forward = rot_lr * view->camera_forward;
        view->camera_up = rot_lr * view->camera_up;

        /*
         * Panning up and down is similar but using the camera left vector.
         * Note that the camera left vector will not change under this rotation, so it is not transformed.
         */
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


    auto aspect_ratio = 16.0f/9.0f;
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

    /*
     * Here is where we store the view object in the window.
     */
    glfwSetWindowUserPointer(window, the_view);

    /*
     * These two function calls 'register' the callbacks.
     * This means that the callback functions will be called by the system when the appropriate events
     *      happen.
     *
     *  Note that the functions names are given without ().
     *  This means that the functions are not being called, it is the functions themselves that are being
     *      passed to the registration function.
     */
    glfwSetKeyCallback(window, general_key_callback);
    glfwSetCursorPosCallback(window, move_callback);
	
    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
        glClear(GL_DEPTH_BUFFER_BIT);

        render();
        glfwSwapBuffers(window);
        /*
         * The glfwPollEvents function will make sure that all callback functions that have been registered
         * will be called.
         *
         * Note that event handling does not overlap rendering.
         * This saves potential problems relating to race conditions.
         */
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
