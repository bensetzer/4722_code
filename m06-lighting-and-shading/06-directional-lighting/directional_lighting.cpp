
/**
 * Starting with the no-lighting project, there are numerous changes needed to support shading.
 *      * A light object is defined
 *      * The light's ambient, diffuse, and specular components must be sent to the fragment shader
 *      * The specular, ambient, and diffuse colors of the surface material of each part must be
 *          sent to the fragment shader
 *      * The surface's shininess and specular strength must be sent to the fragment shader
 *      * Because shading is computed in the view frame (in this implementation)  the model-view
 *          transform must be sent to the vertex shader
 *
 *   Changes are needed in the shaders.  See those for comments
 */


#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_inverse.hpp>

#include <iostream>


#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/artifact.h"
#include "cs4722/view.h"
#include "cs4722/light.h"
#include "../callbacks.h"

static cs4722::view *the_view;
static GLuint program;
static GLuint vao;

// many uniform variable locations to deal with
const static GLint u_ambient_light = 4;  // three components of light
const static GLint u_specular_light = 5;
const static GLint u_diffuse_light = 6;
const static GLint u_ambient_color = 9;  // three components of surface material
const static GLint u_specular_color = 10;
const static GLint u_diffuse_color = 11;
const static GLint u_light_direction = 7; // dposition of the light
const static GLint u_camera_position = 8;
const static GLint u_specular_shininess = 12;     // exponent for sharpening highlights
const static GLint u_specular_strength = 13;      // extra factor to adjust shininess
const static GLint u_m_transform = 1;
const static GLint u_vp_transform = 2;
const static GLint u_normal_transform = 3;

const static GLint b_position = 1;
const static GLint b_normal = 2;


static std::vector<cs4722::artifact*> artifact_list;

static cs4722::light a_light;

void init()
{
    the_view = new cs4722::view();
    the_view->enable_logging = false;
    a_light.ambient_light = cs4722::x11::gray25;
    a_light.light_direction_position = glm::vec4(0,-1,0,0);
    a_light.is_directional = true;

    Shader shader("vertex_shader06.glsl",
                  "fragment_shader06.glsl");
	program = shader.ID;
	glUseProgram(program);

    glEnable(GL_DEPTH_TEST);


	// create a list of shapes that will be shared by the artifacts we have

	const auto color_set = std::vector <cs4722::color>({
			cs4722::x11::white, cs4722::x11::grey50, cs4722::x11::grey75,
		});
	
	auto* shape_list = new std::vector<cs4722::shape*>();
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
                artf->animation_transform.rotation_center =
                        artf->world_transform.matrix() * glm::vec4(0,3,0,1);
                artf->rotation_rate = ((x + y + z) % 12 * M_PI / 24);
                /**
                 * The surface material is set up.
                 * The same color is used for the ambient color and the diffuse color.
                 * The specular color is set to white
                 */
				artf->surface_material.ambient_color = (cs4722::color(
					x * 255 / (number-1), y * 255 / (number - 1), 
					z * 255 / (number - 1), 255));
				artf->surface_material.specular_color = cs4722::x11::white;
				artf->surface_material.diffuse_color = artf->surface_material.ambient_color;
				artf->surface_material.specular_strength = .75;
				artf->surface_material.shininess = 30.0;
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
    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);


    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_normal, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_normal);

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



void display()
{

    glBindVertexArray(vao);
    glUseProgram(program);

    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect,
                                                         the_view->perspective_near);

    auto vp_transform = projection_transform * view_transform;

    glUniform4fv(u_light_direction, 1, glm::value_ptr(a_light.light_direction_position));
    glUniform4fv(u_camera_position, 1, glm::value_ptr(the_view->camera_position));


    static auto last_time = 0.0;
    auto time = glfwGetTime();
	auto delta_time = time - last_time;

	for (auto artf: artifact_list) {

		artf->animate(time, delta_time);
        auto model_transform = artf->animation_transform.matrix() * artf->world_transform.matrix();

        //
        /**
         *      The model-view-projection transform we have been sending to the vertex shader
         *          is here replaced by two separate parts: the model transform and
         *          the view-projection.
         *          These will be combined in the vertex shader.
         *          But, the model transform is needed there to convert to world coordinates
         *          so it's easier to set the transforms up this way
         */
        glUniformMatrix4fv(u_m_transform, 1, GL_FALSE, glm::value_ptr(model_transform));
        glUniformMatrix4fv(u_vp_transform, 1, GL_FALSE, glm::value_ptr(vp_transform));
        glUniformMatrix4fv(u_normal_transform, 1, GL_FALSE,
                           glm::value_ptr(glm::inverseTranspose(model_transform)));


        glUniform4fv(u_ambient_light, 1, a_light.ambient_light.as_float().get());
        glUniform4fv(u_diffuse_light, 1, a_light.diffuse_light.as_float().get());
        glUniform4fv(u_specular_light, 1, a_light.specular_light.as_float().get());
        glUniform4fv(u_ambient_color, 1, artf->surface_material.ambient_color.as_float().get());
        glUniform4fv(u_diffuse_color, 1,  artf->surface_material.diffuse_color.as_float().get());
        glUniform4fv(u_specular_color, 1, artf->surface_material.specular_color.as_float().get());
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Point Lighting", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);
//	cs4722::setup_debug_callbacks();

	init();
    the_view->perspective_aspect = aspect_ratio;

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
