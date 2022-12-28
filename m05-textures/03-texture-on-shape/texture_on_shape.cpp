


#include "GLM/gtc/type_ptr.hpp"
#include <glad/gl.h>

#include "learnopengl/shader.h"

#include "cs4722/artifact.h"
#include "cs4722/view.h"
#include "cs4722/texture_utilities.h"  // include texture utilities

#include "../callbacks.h"

static cs4722::view *the_view;
static GLuint program;
static std::vector<cs4722::artifact*> artifact_list;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_texture_coords = 2;

const static auto u_sampler = 2;




void init()
{

    the_view = new cs4722::view();
    the_view->camera_position = glm::vec3(0,0,1);
//    the_view->enable_logging = true;

//    the_view->set_flup(glm::vec3(-0.256451,-0.0699429,-0.964024),
//                       glm::vec3(-0.96639,0,0.257081),
//                       glm::vec3(-0.0179809,0.997551,-0.0675921),
//                       glm::vec3(-0.161935,0.111801,0.930503));



    Shader shader("vertex_shader03.glsl","fragment_shader03.glsl");
    program = shader.ID;
    glUseProgram(program);



    glEnable(GL_DEPTH_TEST);


    /*
     *   Use one of the texture utilities to load a texture from a file.
     *
     *   The first parameter is a string which gives a path to an image file.
     *   In this example the path is a relative path.
     *   Please only use relative paths in your assignments that you are turning in.
     *
     *   The second parameter is the texture unit you want to assign the texture to.
     *   Make sure your textures are assigned to different texture units.
     */
    cs4722::init_texture_from_file("../media/colorado-river-canyonlands-national-park.jpg", 2);
//    cs4722::init_texture_from_file("../media/citrus-fruit-skin-2523487_1280-pixabay.jpg", 2);

//    auto wrap_type = GL_CLAMP_TO_EDGE;
//    auto wrap_type = GL_MIRRORED_REPEAT;
    auto wrap_type = GL_REPEAT;
//    auto wrap_type = GL_CLAMP_TO_BORDER;
//    auto wrap_type = GL_MIRROR_CLAMP_TO_EDGE;
    // set the wrap to be the same in both coordinate directions.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_type);





    /*
     * Back to creating a shape and using the buffer utilities.
     */

//    cs4722::shape* b1 = new cs4722::sphere(15,30);
//    cs4722::shape* b1 = new cs4722::torus(.5, 15,30);
//    cs4722::shape* b1 = new cs4722::cylinder(.5, 30);
    cs4722::block* b1 = new cs4722::block();
//     b1->texture_scale = 4.0;

    auto* artf1 = new cs4722::artifact_rotating();
    artf1->the_shape = (b1);
    artf1->world_transform.scale = (glm::vec3(.6f, .6f, .6f));
    artf1->world_transform.translate = (glm::vec3(0, 0, -.5));
    artf1->world_transform.rotation_axis = glm::vec3(0,1,0);
    artf1->world_transform.rotation_angle = M_PI;
    artf1->animation_transform.rotation_axis = (glm::vec3(1, 0, 1));
    artf1->animation_transform.rotation_center = // artf1->world_transform.translate;
            artf1->world_transform.matrix() * glm::vec4(0,0,0, 1);
    artf1->rotation_rate = (M_PI / 3);
    artf1->texture_unit = 2;

    artifact_list.push_back(artf1);


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
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_texture_coords, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_texture_coords);

    for (auto artf : artifact_list) {
        if(!artf->shape_is_shared) {
            auto pos = artf->the_shape->positions();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            pos->data());
            delete pos;

            auto texture_coords = artf->the_shape->texture_coordinates();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            2 * 4 * artf->the_shape->buffer_start,
                            2 * 4 * artf->the_shape->buffer_size,
                            texture_coords->data());
            delete texture_coords;
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


    for (auto artf : artifact_list) {

        artf->animate(time, delta_time);

        auto model_transform = artf->animation_transform.matrix() * artf->world_transform.matrix();
        auto transform = vp_transform * model_transform;
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform1i(u_sampler, artf->texture_unit);

        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start, artf->the_shape->buffer_size);
    }
}


int
main(int argc, char** argv)
{
    glfwInit();


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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Texture on Shape", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
//    cs4722::setup_debug_callbacks();
    init();
    the_view->perspective_aspect = aspect_ratio;


    glfwSetWindowUserPointer(window, the_view);

    glfwSetWindowUserPointer(window, the_view);
    glfwSetKeyCallback(window, general_key_callback);
    glfwSetCursorPosCallback(window, move_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

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
