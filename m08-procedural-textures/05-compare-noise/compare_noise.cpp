


#include <GLM/vec4.hpp>
#include <GLM/vec2.hpp>
#include <glm/gtc/noise.hpp>
#include <GLM/gtc/type_ptr.hpp>


#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"


#include "cs4722/view.h"
#include "cs4722/artifact.h"
#include "../callbacks.h"

#include "FastNoiseLite.h"

static GLuint program;
static cs4722::view *the_view;
static std::vector<cs4722::artifact*> parts_list;


const static auto b_position = 1;
const static auto b_texture_coords = 2;

static GLint u_transform = 1;
static GLint u_sampler = 2;
static GLint u_texture_unit = 3;



void init()
{

    the_view = new cs4722::view();
    the_view->set_flup(glm::vec3(0, 0, -1), glm::vec3(-1, 0, 0),
                       glm::vec3(0, 1, 0), glm::vec3(0.3, 0.3, 0.9));

    Shader shader("vertex_shader05.glsl",
                  "fragment_shader05.glsl");
	program = shader.ID;
	glUseProgram(program);

	glEnable(GL_DEPTH_TEST);


	auto* rectangle_shape = new cs4722::rectangle(1, 1);

	auto scale = 0.45f;
	
	auto part = new cs4722::artifact();
    part->the_shape = rectangle_shape;
    part->world_transform.translate = glm::vec3(-0.5, -0.5, 0.0);
    part->world_transform.scale = glm::vec3(scale, scale, scale );
    part->texture_unit = 3;
	parts_list.push_back(part);


    part = new cs4722::artifact();
    part->the_shape = rectangle_shape;
    part->world_transform.translate = glm::vec3(0.5, -0.5, 0.0);
    part->world_transform.scale = glm::vec3(scale, scale, scale);
    part->texture_unit = 2;
	parts_list.push_back(part);

    part = new cs4722::artifact();
    part->the_shape = rectangle_shape;
    part->world_transform.translate = glm::vec3(0.5, 0.5, 0.0);
    part->world_transform.scale = glm::vec3(scale, scale, scale);
    part->texture_unit = 1;
	parts_list.push_back(part);

    part = new cs4722::artifact();
    part->the_shape = rectangle_shape;
    part->world_transform.translate = glm::vec3(-0.5, 0.5, 0.0);
    part->world_transform.scale = glm::vec3(scale, scale, scale);
    part->texture_unit = 0;
	parts_list.push_back(part);

    part = new cs4722::artifact();
    part->the_shape = rectangle_shape;
    part->world_transform.translate = glm::vec3(0, 0, 0.0);
    part->world_transform.scale = glm::vec3(scale, scale, scale);
    part->texture_unit = 4;
	parts_list.push_back(part);

    auto total_vertex_count = 0;
    for(auto *artf : parts_list) {
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

    for (auto artf : parts_list) {
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



void init_texture2D() {


	const auto texture_size = 512;
	auto* texture_data = new GLubyte[4 * texture_size * texture_size];


	auto const number_of_octaves = 4;


	auto  frequency = 4.0f;
	auto amp = 0.5f;
	//int inc = 0;

	// auto* noise_gen = new SimplexNoise();


	for (auto f = 0; f < number_of_octaves; ++f)
	{
		auto ni = glm::vec2(0.0);
		auto inc = 1.0f / (texture_size / frequency);

		auto* ptr = texture_data;

		for (auto i = 0; i < texture_size; ++i)
		{
			for (auto j = 0; j < texture_size; ++j)
			{
				// *(ptr + f) = static_cast<GLubyte>(glm::perlin(ni) * amp * 128.0f);
				// *(ptr + f) = static_cast<GLubyte>((glm::simplex(ni) + 1.0f) * amp * 128.0f);
				// The following seems to be what was intended by the code in the book
				//      ni.y is incremented too much in the text code, if I'm reading it correctly
				// *(ptr + f) = static_cast<GLubyte>((glm::simplex(glm::vec2(i, j) * inc) + 1.0f) * amp * 128.0f);
				// don't use amplitude here
				*(ptr + f) = static_cast<GLubyte>((glm::simplex(glm::vec2(i, j) * inc) + 1.0f) * 128.0f);
				//  next makes all octaves the same amplitude, for easier comparison
				//*(ptr + f) = static_cast<GLubyte>((glm::simplex(glm::vec2(i, j) * inc) + 1.0f) * 128.0f);
				ptr += 4;
				ni.y += inc;
			}
			ni.x += inc;
		}


		frequency *= 2;
		++inc;
		amp *= 0.5;
	}




	// initialize texture and sampler

	auto internal_format = GL_RGBA8;
	auto external_format = GL_RGBA;

    GLuint texture;
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);

//    int texture_width, texture_height, n;
    glTexImage2D(GL_TEXTURE_2D,
                 0, // level
                 internal_format,
                 texture_size, texture_size,
                 0, // border
                 external_format,
                 GL_UNSIGNED_BYTE,
                 texture_data
    );
    delete texture_data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);




//	GLuint texture;
//	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
//
//	auto number_of_levels = 1;

//
//
//	glTextureStorage2D(texture, number_of_levels, internal_format, texture_size, texture_size);
//	glTextureSubImage2D(texture, 0, 0, 0, texture_size, texture_size,
//		external_format, GL_UNSIGNED_BYTE, texture_data);
//	delete texture_data;
//	glBindTextureUnit(2, texture);
//
//	glTextureParameterfv(texture, GL_TEXTURE_BORDER_COLOR, cs4722::x11::aquamarine.as_float());
//	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	// auto mag_filter = GL_NEAREST;
//	auto mag_filter = GL_LINEAR;
//	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, mag_filter);
//	// auto wrap_type = GL_CLAMP_TO_EDGE;
//	// auto wrap_type = GL_MIRRORED_REPEAT;
//	auto wrap_type = GL_REPEAT;
//	// auto wrap_type = GL_CLAMP_TO_BORDER;
//	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, wrap_type);
//	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, wrap_type);


	auto sampler_loc = glGetUniformLocation(program, "sampler");

	glUniform1i(sampler_loc, 2);

}




void init_texture2D_alt() {


	const auto texture_size = 512;
	auto* texture_data = new GLubyte[4 * texture_size * texture_size];


	auto const number_of_octaves = 4;


	auto  frequency = 4.0f;
	auto amp = 1.0f;// 0.5f;
	//int inc = 0;

	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);


	for (auto f = 0; f < number_of_octaves; ++f)
	{
		noise.SetFrequency(frequency);

		// auto inc = 1.0f / (texture_size / frequency);
		auto inc = 1.0f / texture_size;

		auto* ptr = texture_data;

		for (auto i = 0; i < texture_size; ++i)
		{
			for (auto j = 0; j < texture_size; ++j)
			{
				auto sample = noise.GetNoise(i * inc, j * inc);
				*(ptr + f) = static_cast<GLubyte>((sample + 1.0f) * amp * 128.0f);

				ptr += 4;
			}
		}


		frequency *= 2;
		amp *= 0.5f;
	}




	// initialize texture and sampler

    auto internal_format = GL_RGBA8;
    auto external_format = GL_RGBA;

    GLuint texture;
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);

//    int texture_width, texture_height, n;
    glTexImage2D(GL_TEXTURE_2D,
                 0, // level
                 internal_format,
                 texture_size, texture_size,
                 0, // border
                 external_format,
                 GL_UNSIGNED_BYTE,
                 texture_data
    );
    delete texture_data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


//
//	GLuint texture;
//	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
//
//	auto number_of_levels = 1;
//	auto internal_format = GL_RGBA8;
//	auto external_format = GL_RGBA;
//
//
//	glTextureStorage2D(texture, number_of_levels, internal_format, texture_size, texture_size);
//	glTextureSubImage2D(texture, 0, 0, 0, texture_size, texture_size,
//		external_format, GL_UNSIGNED_BYTE, texture_data);
//	delete texture_data;
//	glBindTextureUnit(2, texture);
//
//	glTextureParameterfv(texture, GL_TEXTURE_BORDER_COLOR, cs4722::x11::aquamarine.as_float());
//	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	// auto mag_filter = GL_NEAREST;
//	auto mag_filter = GL_LINEAR;
//	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, mag_filter);
//	// auto wrap_type = GL_CLAMP_TO_EDGE;
//	// auto wrap_type = GL_MIRRORED_REPEAT;
//	auto wrap_type = GL_REPEAT;
//	// auto wrap_type = GL_CLAMP_TO_BORDER;
//	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, wrap_type);
//	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, wrap_type);


//	auto sampler_loc = glGetUniformLocation(program, "sampler");

	glUniform1i(u_sampler, 2);

}


void display()
{
    static auto last_time = 0.0;
	auto time = glfwGetTime();
	auto delta_time = time - last_time;
	last_time = time;

    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect, the_view->perspective_near);
    auto vp_transform = projection_transform * view_transform;





	for (auto* obj : parts_list) {
		obj->animate(time, delta_time);
        auto model_transform = obj->animation_transform.matrix() * obj->world_transform.matrix();
		auto transform = vp_transform * model_transform;
		glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(transform));
		glUniform1i(u_texture_unit, obj->texture_unit);
		glDrawArrays(GL_TRIANGLES, obj->the_shape->buffer_start,
			obj->the_shape->buffer_size);
	}
}


int
main(int argc, char** argv)
{
	glfwInit();

    // set the OpenGL version to use: 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Compare Noise", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);
    init();

	// init_texture2D();
    init_texture2D_alt();

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
