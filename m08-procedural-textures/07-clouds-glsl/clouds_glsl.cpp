


#include <GLM/vec4.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_inverse.hpp>


#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/view.h"
#include "cs4722/artifact.h"
#include "cs4722/light.h"
#include "../callbacks.h"

static GLuint program;
static cs4722::view* the_view;
static std::vector<cs4722::artifact*> artifact_list;
static cs4722::light the_light;

const static GLint mc_vertex = 1;
const static GLint mc_normal = 2;

void init()
{
    Shader shader("vertex_shader07.glsl",
                  "fragment_shader07.glsl");
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);



    auto* shape_list = new std::vector<cs4722::shape*>();
    auto pos = 0;
    shape_list->push_back(new cs4722::sphere());
    shape_list->push_back(new cs4722::block());
    shape_list->push_back(new cs4722::torus());
    shape_list->push_back(new cs4722::cylinder());
    auto numshp = shape_list->size();



    auto number = 4;
    auto d = 20.0f / (2 * number + 1);
    auto radius = d / 4;
    auto base = -number * d / 2 + radius;

    for (auto x = 0; x < number; ++x)
    {
        for (auto y = 0; y < number; ++y)
        {
            for (auto z = 0; z < number; ++z)
            {
                auto* artf = new cs4722::artifact_rotating();
                artf->the_shape = (shape_list->at((x + y + z) % numshp));
                artf->world_transform.translate = (glm::vec3(base + x * d, base + y * d, base + z * d));
                artf->world_transform.scale = (glm::vec3(radius, radius, radius));
                artf->animation_transform.rotation_axis = (glm::vec3(x + 1, y + 1, z + 1));
                artf->rotation_rate = ((x + y + z) % 12 * M_PI / 24);
                artf->surface_material.diffuse_color = (cs4722::color(
                        x * 255 / (number - 1), y * 255 / (number - 1),
                        z * 255 / (number - 1), 255));
                artf->surface_material.ambient_color = cs4722::color(255 - artf->surface_material.diffuse_color.r,
                                                255 - artf->surface_material.diffuse_color.g,
                                                255 - artf->surface_material.diffuse_color.b, 255);
                artifact_list.push_back(artf);
            }
        }
    }

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
    glVertexAttribPointer(mc_vertex, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mc_vertex);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(mc_normal, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mc_normal);


    for (auto artf : artifact_list) {
        if(!artf->shape_is_shared) {
            auto pos = artf->the_shape->positions();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            pos->data());
            delete pos;

            auto nrm = artf->the_shape->normals();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            nrm->data());
            delete nrm;


        }
    }


}



void
display()
{


    // vertex shader
    const auto mv_transform_loc = glGetUniformLocation(program, "MVMatrix");
    //const auto mvp_transform = glGetUniformLocation(the_scene->program, " MVPMatrix");
    const auto n_transform = glGetUniformLocation(program, "NormalMatrix");
    const auto light_position = glGetUniformLocation(program, "LightPos");
    const auto cloud_scale = glGetUniformLocation(program, "Scale");

    const auto SkyColor = glGetUniformLocation(program, "SkyColor");
    const auto CloudColor = glGetUniformLocation(program, "CloudColor");

    glUniform4fv(light_position, 1, glm::value_ptr(the_light.light_direction_position));


    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect, the_view->perspective_near);
//    auto vp_transform = projection_transform * view_transform;


//    const auto view_transform = the_view->look_at();
//    const auto projection_transform = the_view->projection();
    auto p_transform = glGetUniformLocation(program, "p_transform");
    glUniformMatrix4fv(p_transform, 1, GL_FALSE,
                       glm::value_ptr(projection_transform));

    static auto last_time = 0.0;
    auto time = glfwGetTime();
    auto delta_time = time - last_time;

    for (auto *artf: artifact_list) {

        artf->animate(time, delta_time);

        auto model_transform = artf->animation_transform.matrix() * artf->world_transform.matrix();
        auto mv_transform = view_transform * model_transform;
        glUniformMatrix4fv(mv_transform_loc, 1, GL_FALSE, glm::value_ptr(mv_transform));
        glUniform4fv(SkyColor, 1, artf->surface_material.diffuse_color.as_float().get());
        glUniform4fv(CloudColor, 1, artf->surface_material.ambient_color.as_float().get());
        glUniformMatrix4fv(n_transform, 1, GL_FALSE,
                           glm::value_ptr(glm::inverseTranspose(mv_transform)));

        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start,
                     artf->the_shape->buffer_size);

    }
}


int
main(int argc, char** argv)
{

    the_view = new cs4722::view();
    the_view->set_camera_position(glm::vec3(0, 0, 1));

	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Clouds GLSL", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);
//    cs4722::setup_debug_callbacks();

	the_light.light_direction_position = glm::vec4(0, -1, 0, 1);

	init();
    the_view->perspective_aspect = aspect_ratio;

	const auto cloud_scale = glGetUniformLocation(program, "Scale");
	glUniform1f(cloud_scale, 0.2f);
	const auto SkyColor = glGetUniformLocation(program, "SkyColor");
	const auto CloudColor = glGetUniformLocation(program, "CloudColor");
	glUniform4fv(SkyColor, 1, glm::value_ptr(glm::vec4(0.0, 0.0, 0.8, 1.0)));
	glUniform4fv(CloudColor, 1, glm::value_ptr(glm::vec4(0.8, 0.8, 0.8, 1.0)));

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
