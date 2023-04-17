

#include <ctime>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_inverse.hpp>
#include <glad/gl.h>

#include "learnopengl/shader.h"
#include "cs4722/artifact.h"
#include "cs4722/view.h"
#include "cs4722/shape.h"
#include "cs4722/texture_utilities.h"
#include "cs4722/light.h"

//#include "checkerboard_texture.h"

//#define LOG_FLUP 1

static cs4722::view *the_view;
static cs4722::light a_light;
static GLuint program;
static std::vector<cs4722::artifact*> artf_list;


// locations of variables in the shaders
const static GLint u_m_transform = 1;
const static GLint u_vp_transform = 2;
const static GLint u_normal_transform = 3;

const static auto u_sampler = 4;
const static auto u_sampler2 = 5;
const static auto u_surface_effect = 6;
// light
const static GLint u_ambient_light = 7;  // three components of light
const static GLint u_specular_light = 8;
const static GLint u_diffuse_light = 9;
// material
const static GLint u_ambient_color = 10;  // three components of surface material
const static GLint u_specular_color = 11;
const static GLint u_diffuse_color = 12;
const static GLint u_specular_shininess = 13;     // exponent for sharpening highlights
const static GLint u_specular_strength = 14;      // extra factor to adjust shininess
// light and camera
const static GLint u_light_position = 15; // position of the light
const static GLint u_camera_position = 16;


const static auto b_position = 1;
const static auto b_color = 2;
const static auto b_texture_coord = 3;
const static auto b_normal = 4;

static height_field_fnl *terrain_height_field;
static glm::vec3 terrain_scale = glm::vec3(100, 10, 100);
static glm::vec3 terrain_translate = glm::vec3(0,0,0);
static bool movement_restricted = false;
static double max_height = .9 * terrain_scale.y + terrain_translate.y;




void init()
{
    the_view = new cs4722::view();
    the_view->set_flup(glm::vec3(-0.985727,-0.0799147,0.148175),
                       glm::vec3(0.148651,0,0.98889),
                       glm::vec3(-0.0790268,0.996801,0.0118794),
                       glm::vec3(59.1352,22.4733,30.813));
//    a_light.ambient_light = cs4722::x11::gray25;
    a_light.is_directional = false;
    a_light.light_direction_position = glm::vec4(50,50,50,1);

    Shader shader("vertex_shader07.glsl","fragment_shader07.glsl");
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);





    terrain_height_field = new height_field_fnl(200, 200);
//    terrain_height_field->frequency = 1.5;
    terrain_height_field->frequency = 1.25;
    terrain_height_field->seed = 12345;
    terrain_height_field->seed = 786;
//    terrain_height_field->seed = clock();
    printf("height field seed %d\n", terrain_height_field->seed);
    terrain_height_field->fractal_type = FastNoiseLite::FractalType_Ridged;
    terrain_height_field->noise_type = FastNoiseLite::NoiseType_OpenSimplex2;
    terrain_height_field->number_of_octaves = 8;
    terrain_height_field->ping_pong_strength = 2;
    terrain_height_field->generate();
    cs4722::shape* b = new cs4722::terrain(terrain_height_field);

    auto* artf = new cs4722::artifact();
    artf->the_shape = b;

    artf->world_transform.scale = terrain_scale;
    artf->world_transform.translate = terrain_translate;
    artf->surface_effect = 0;
    artf_list.push_back(artf);

    terrain_height_field->scale = terrain_scale;
    terrain_height_field->translate = terrain_translate;


    cs4722::shape* shape = new cs4722::torus();
    auto artifact_scale = glm::vec3(2,2,2);

    for(auto x = 1; x <= 99; x += 5 ) {
        for(auto z = 1; z <= 99; z += 5 ) {
            auto y = terrain_height_field->altitude(x, z) + artifact_scale.y;
            auto *artf = new cs4722::artifact_rotating();
            artf->the_shape = shape;
            artf->world_transform.translate = glm::vec3(x,y,z);
            artf->world_transform.scale = artifact_scale;
            artf->animation_transform.rotation_axis = glm::vec3(0,1,0);
            artf->animation_transform.rotation_center = artf->world_transform.translate;
            artf->rotation_rate = M_PI / 5;
            artf->surface_effect = (x % 2) + 1;
            artf->texture_unit = (z % 2) + 1;
//            artf->texture_unit = 4;
            artf->surface_material.shininess = x * 30.0 / 100.0;
            artf->surface_material.specular_strength = z * 3.0 / 100.0;
            artf_list.push_back(artf);
        }
    }



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
    const int number_of_buffers = 4;
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

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_texture_coord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_texture_coord);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_normal, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_normal);

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


            auto texture_coords = artf->the_shape->texture_coordinates();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 2 * artf->the_shape->buffer_start,
                            4 * 2 * artf->the_shape->buffer_size,
                            texture_coords->data());
            delete texture_coords;

            auto normal = artf->the_shape->positions();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            normal->data());
            delete normal;


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
    glUniformMatrix4fv(u_vp_transform, 1, GL_FALSE, glm::value_ptr(vp_transform));

    glUniform4fv(u_light_position, 1, glm::value_ptr(a_light.light_direction_position));
    glUniform3fv(u_camera_position, 1, glm::value_ptr(the_view->camera_position));



    static auto last_time = 0.0;
    auto time = glfwGetTime();
    auto delta_time = time - last_time;
    last_time = time;


    for (auto artf : artf_list) {

        artf->animate(time, delta_time);

        auto model_transform = artf->animation_transform.matrix() * artf->world_transform.matrix();
        glUniformMatrix4fv(u_m_transform, 1, GL_FALSE, glm::value_ptr(model_transform));
        auto transform = vp_transform * model_transform;
        glUniformMatrix4fv(u_normal_transform, 1, GL_FALSE,
                           glm::value_ptr(glm::inverseTranspose(model_transform)));

        glUniform4fv(u_diffuse_color, 1, artf->surface_material.diffuse_color.as_float().get());
        glUniform1i(u_sampler, artf->texture_unit);
        glUniform1i(u_surface_effect, artf->surface_effect);

        glUniform4fv(u_ambient_light, 1, a_light.ambient_light.as_float().get());
        glUniform4fv(u_diffuse_light, 1, a_light.diffuse_light.as_float().get());
        glUniform4fv(u_specular_light, 1, a_light.specular_light.as_float().get());
        glUniform4fv(u_ambient_color, 1, artf->surface_material.ambient_color.as_float().get());
        /*
         * The diffuse color of the material is not used in the fragment shader.
         * So, the shader compiler optimizes the variable away.
         * That means that the following statement causes an error when it executes because
         * there's no variable there.
         * So, we comment this out for this example.
         */
//        glUniform4fv(u_diffuse_color, 1,  artf->surface_material.diffuse_color.as_float().get());
        glUniform4fv(u_specular_color, 1, artf->surface_material.specular_color.as_float().get());
        glUniform1f(u_specular_shininess, artf->surface_material.shininess);
        glUniform1f(u_specular_strength, artf->surface_material.specular_strength);
        
        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start, artf->the_shape->buffer_size);
       

    }
}


static float lr_scale = .1f;
static float fb_scale = .1f;
static float ud_scale = .1f;

static float lr_pan_scale = .01f;
static float ud_pan_scale = lr_pan_scale;




void general_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /*
    * These variables are used to hold the window height and width and position to use when
    *  exiting full screen.
    */
    static int old_width, old_height, old_xpos, old_ypos;
    auto position0 = the_view->camera_position;
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch(key) {
            case GLFW_KEY_A:
            case GLFW_KEY_LEFT:
                the_view->camera_position += lr_scale * the_view->camera_left;
                break;
            case GLFW_KEY_D:
            case GLFW_KEY_RIGHT:
                the_view->camera_position -= lr_scale * the_view->camera_left;
                break;
            case GLFW_KEY_W:
                the_view->camera_position += fb_scale * the_view->camera_forward;
                break;
            case GLFW_KEY_S:
                the_view->camera_position -= fb_scale * the_view->camera_forward;
                break;
            case GLFW_KEY_UP:
                the_view->camera_position += ud_scale * the_view->camera_up;
                break;
            case GLFW_KEY_DOWN:
                the_view->camera_position -= ud_scale * the_view->camera_up;
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_F11:
                // only a key press, not repeats as that could cause chaotic window/full-screen switching
                if(action == GLFW_PRESS) {
                    if (!glfwGetWindowMonitor(window)) {
                        glfwGetWindowPos(window, &old_xpos, &old_ypos);
                        glfwGetWindowSize(window, &old_width, &old_height);
                        auto monitor = glfwGetPrimaryMonitor();
                        const auto* mode = glfwGetVideoMode(monitor);
                        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                    }
                    else  {
                        glfwSetWindowMonitor(window, nullptr, old_xpos, old_ypos, old_width, old_height, 0);
                    }
                }
                break;
            default: ;
                // nothing to do on other keys
        }
    }

    if(movement_restricted) {
        // code restricting the movement goes in here.
        the_view->camera_position.y =
                terrain_height_field->altitude(the_view->camera_position.x, the_view->camera_position.z) + 1;
        if(the_view->camera_position.y > max_height
               || the_view->camera_position.x > terrain_translate.x + terrain_scale.x
               || the_view->camera_position.z > terrain_translate.z + terrain_scale.z
               || the_view->camera_position.x < 0
               || the_view->camera_position.z < 0) {
            the_view->camera_position = position0;
        }
    }
#ifdef LOG_FLUP
    the_view->display_parameters();
#endif
}


void window_size_callback(GLFWwindow* window, int width, int height)
{
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
    int widthfb, heightfb;
    glfwGetFramebufferSize(window, &widthfb, &heightfb);
    glViewport(0, 0, widthfb, heightfb);
    view->perspective_aspect =
        (static_cast<float>(widthfb) / static_cast<float>(heightfb));

}



void move_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto cl0 = the_view->camera_left;
    auto cf0 = the_view->camera_forward;
    auto cu0 = the_view->camera_up;

    static auto last_x = 0.0f;
    static auto last_y = 0.0f;
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
     auto dx = xpos - last_x;
     auto dy = ypos - last_y;
     last_x = xpos;
     last_y = ypos;


    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
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
    if(movement_restricted) {
        if(glm::dot(glm::vec3(0,1,0), the_view->camera_up) < 0) {
            the_view->camera_up = cu0;
            the_view->camera_forward  = cf0;
            the_view->camera_left = cl0;
        }
    }
#ifdef LOG_FLUP
    the_view->display_parameters();
#endif
}


int
main(int argc, char** argv)
{
    glfwInit();

    // set the OpenGL version to use: 4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    auto aspect_ratio = 1600.0f/900.0f;
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
