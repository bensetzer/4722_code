
/*
 * This example shows five shapes without using the view and projection transformations we
 * have discussed in Module 3.
 *
 * Even though the shapes are different distances from the camera they all look the same size.
 *
 */

#include "GLM/gtc/type_ptr.hpp"
#include <glad/gl.h>

#include "learnopengl/shader.h"

#include <cs4722/artifact.h>
#include "cs4722/view.h"



static GLuint program;
static std::vector<cs4722::artifact*> artifact_list;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;



void init()
{


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
    for (auto artf : artifact_list) {
        artf->animate(time, delta_time);

        auto model_matrix = artf->animation_transform.matrix() * artf->world_transform.matrix();

        auto transform = model_matrix;
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
