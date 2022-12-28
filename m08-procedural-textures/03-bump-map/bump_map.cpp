


#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_inverse.hpp>

#include <iostream>


#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/view.h"
#include "cs4722/x11.h"
#include "cs4722/shape.h"
#include "cs4722/artifact.h"
#include "cs4722/light.h"
#include "../callbacks.h"

static cs4722::view *the_view;
static GLuint program;
static GLuint vao;

const static GLuint b_position = 1;
const static GLuint b_normal = 2;
const static GLuint b_tangent = 3;
const static GLuint b_texture_coord = 4;

const static GLuint u_mv_transform = 1;
const static GLuint u_v_transform = 2;
const static GLuint u_p_transform = 3;
const static GLuint u_normal_transform = 4;
const static GLuint u_light_position = 5;

const static GLuint u_ambient_product = 6;
const static GLuint u_specular_product = 7;
const static GLuint u_diffuse_product = 8;
const static GLuint u_specular_shininess = 9;
const static GLuint u_specular_strength = 10;
const static GLuint u_surface_color = 11;
const static GLuint u_bump_density = 12;
const static GLuint u_bump_size = 13;
const static GLuint u_specular_factor = 14;


static std::vector<cs4722::artifact*> artifact_list;

static cs4722::light a_light;

void init()
{
    the_view = new cs4722::view();
    the_view->enable_logging = false;
    a_light.ambient_light = cs4722::x11::gray25;
    a_light.light_direction_position = glm::vec4(0, 5, 5, 1);
        ///< in world coordinates

    Shader shader("vertex_shader03.glsl",
                  "fragment_shader03.glsl");
	program = shader.ID;
	glUseProgram(program);

	glEnable(GL_DEPTH_TEST);


	// create a list of shapes that will be shared by the objects we have

	const auto color_set = std::vector <cs4722::color>({
			cs4722::x11::white, cs4722::x11::grey50, cs4722::x11::grey75,
		});
	
	auto* shape_list = new std::vector<cs4722::shape*>();
//	shape_list->push_back(new cs4722::sphere());
	shape_list->push_back(new cs4722::block());
	shape_list->push_back(new cs4722::torus());
//	shape_list->push_back(new cs4722::cylinder());
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
                artf->animation_transform.rotation_center =
                        artf->world_transform.matrix() * glm::vec4(0,3,0,1);
                artf->rotation_rate = ((x + y + z) % 12 * M_PI / 24);
				artf->surface_material.ambient_color = (cs4722::color(
					x * 255 / (number-1), y * 255 / (number - 1), 
					z * 255 / (number - 1), 255));
				artf->surface_material.specular_color = cs4722::x11::white;
				artf->surface_material.diffuse_color = artf->surface_material.ambient_color;
				artf->surface_material.specular_strength = 30.0;
				artf->surface_material.shininess = 15.0;
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
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_texture_coord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_texture_coord);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_normal, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_normal);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_tangent, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_tangent);


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

            auto nrm = artf->the_shape->normals();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            nrm->data());
            delete nrm;

            auto tng = artf->the_shape->tangents();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            tng->data());
            delete tng;
        }
    }
}



void display()
{
    static auto last_time = 0.0;

    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glBindVertexArray(vao);
    glUseProgram(program);

    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect,
                                                         the_view->perspective_near);

    glUniform4fv(u_light_position, 1,
                 glm::value_ptr(view_transform * a_light.light_direction_position));


    auto time = glfwGetTime();
	auto delta_time = time - last_time;

	for (auto artf: artifact_list) {

		artf->animate(time, delta_time);
        auto mv_transform = view_transform *
                artf->animation_transform.matrix() * artf->world_transform.matrix();

        glUniformMatrix4fv(u_mv_transform, 1, GL_FALSE, glm::value_ptr(mv_transform));
        glUniformMatrix4fv(u_p_transform, 1, GL_FALSE, glm::value_ptr(projection_transform));
        glUniformMatrix4fv(u_normal_transform, 1, GL_FALSE,
                           glm::value_ptr(glm::inverseTranspose(mv_transform)));

        glm::vec4 ambient_product = *a_light.ambient_light.as_vec4()
                * *artf->surface_material.ambient_color.as_vec4();
        glm::vec4 diffuse_product = *a_light.diffuse_light.as_vec4()
                                    * *artf->surface_material.diffuse_color.as_vec4();
        glm::vec4 specular_product = *a_light.specular_light.as_vec4()
                                    * *artf->surface_material.specular_color.as_vec4();

        glUniform4fv(u_ambient_product, 1, glm::value_ptr(ambient_product));
        glUniform4fv(u_diffuse_product, 1, glm::value_ptr(diffuse_product));
        glUniform4fv(u_specular_product, 1,glm::value_ptr(specular_product));
        glUniform1f(u_specular_shininess, artf->surface_material.shininess);
        glUniform1f(u_specular_strength, artf->surface_material.specular_strength);


        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start,
			artf->the_shape->buffer_size);
		
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Bump Mapping", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);
//	cs4722::setup_debug_callbacks();

	init();

	glfwSetWindowUserPointer(window, the_view);
    glfwSetKeyCallback(window, general_key_callback);
    glfwSetCursorPosCallback(window, move_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

	while (!glfwWindowShouldClose(window))
	{
        glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray25.as_float().get());
        glClear(GL_DEPTH_BUFFER_BIT);

        display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
}
