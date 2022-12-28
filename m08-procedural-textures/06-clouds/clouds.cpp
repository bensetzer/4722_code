


#include <GLM/vec4.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <GLM/gtc/matrix_inverse.hpp>


#include <iostream>

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include "FastNoiseLite.h"

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

    the_light.light_direction_position = glm::vec4(0, -1, 0, 1);


    Shader shader("vertex_shader06.glsl",
                  "fragment_shader06.glsl");
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);




    // create a list of shapes that will be shared by the objects we have

//    const auto color_set = std::vector <cs4722::color>({
//                                                               cs4722::x11::white, cs4722::x11::grey50, cs4722::x11::grey75,
//                                                       });

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



//
//void init_texture3D() {
//
//
//	const auto texture_size = 128;
//	auto* texture_data = new GLubyte[4 * texture_size * texture_size * texture_size];
//
//
//	auto const number_of_octaves = 4;
//
//
//	auto  frequency = 4.0f;
//	auto amp = 0.5f;
//	//int inc = 0;
//
//	// auto* noise_gen = new SimplexNoise();
//
//	FastNoiseLite noise;
//	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
//	noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXZPlanes);
//
//	for (auto f = 0; f < number_of_octaves; ++f)
//	{
//
//		noise.SetFrequency(frequency);
//
//		// auto ni = glm::vec3(0.0);
//		// auto inc = 1.0f / (texture_size / frequency);
//		auto inc =  4.0f / texture_size;
//
//		auto* ptr = texture_data;
//
//		for (auto i = 0; i < texture_size; ++i)
//		{
//			for (auto j = 0; j < texture_size; ++j)
//			{
//				for (auto k = 0; k < texture_size; ++k)
//				{
//					//(GLubyte)(((noise3(ni)+1.0) * amp) * 128.0);
//
//					// *(ptr + f) = static_cast<GLubyte>((glm::simplex(glm::vec3(i, j, k) * inc) + 1.0f) * amp * 128.0f);
//					// *(ptr + f) = static_cast<GLubyte>(glm::perlin(ni / frequency) * amp * 128.0f);
//					// ni.z += inc;
//
//					auto sample = noise.GetNoise(i * inc, j * inc, k * inc);
//					*(ptr + f) = static_cast<GLubyte>((sample + 1.0f) * amp * 128.0f);
//
//					ptr += 4;
//				}
//				// ni.y += inc;
//			}
//			// ni.x += inc;
//		}
//
//
//		frequency *= 2;
//		++inc;
//		amp *= 0.5;
//	}
//
//
//
//
//	// initialize texture and sampler
//
////	GLuint texture;
////	glCreateTextures(GL_TEXTURE_3D, 1, &texture);
//
//	auto number_of_levels = 1;
//	auto internal_format = GL_RGBA8;
//	auto external_format = GL_RGBA;
//
//
////	glTextureStorage3D(texture, number_of_levels, internal_format, texture_size, texture_size, texture_size);
////	glTextureSubImage3D(texture, 0, 0, 0, 0, texture_size, texture_size, texture_size,
////		external_format, GL_UNSIGNED_BYTE, texture_data);
////	delete texture_data;
////	glBindTextureUnit(3, texture);
////
////	glTextureParameterfv(texture, GL_TEXTURE_BORDER_COLOR, cs4722::x11::aquamarine.as_float());
////	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
////	// auto mag_filter = GL_NEAREST;
////	auto mag_filter = GL_LINEAR;
////	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, mag_filter);
////	// auto wrap_type = GL_CLAMP_TO_EDGE;
////	// auto wrap_type = GL_MIRRORED_REPEAT;
////	auto wrap_type = GL_REPEAT;
////	// auto wrap_type = GL_CLAMP_TO_BORDER;
////	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, wrap_type);
////	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, wrap_type);
//
//    GLuint texture;
//    glGenTextures(1, &texture);
//
//    glActiveTexture(GL_TEXTURE3);
//    glBindTexture(GL_TEXTURE_3D, texture);
//
//    glTexImage3D(GL_TEXTURE_3D,
//                 0, // level
//                 internal_format,
//                 texture_size, texture_size, texture_size,
//                 0, // border
//                 external_format,
//                 GL_UNSIGNED_BYTE,
//                 texture_data
//    );
//    delete texture_data;
//
//
//
//
//    auto sampler_loc = glGetUniformLocation(program, "Noise");
//	std::cout << "sampler_loc " << sampler_loc << std::endl;
//	glUniform1i(sampler_loc, 3);
//
//}


void init_texture3D_F() {


    const auto texture_size = 128;
    auto* texture_data = new GLubyte[4 * texture_size * texture_size * texture_size];


    auto const number_of_octaves = 1;


    auto  frequency = 4.0f;
    auto amp = 0.5f;
    //int inc = 0;

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(frequency);

    for (auto f = 0; f < number_of_octaves; ++f)
    {
        // auto ni = glm::vec3(0.0);
        auto inc = 1.0f / (texture_size / frequency);

        auto* ptr = texture_data;

        for (auto i = 0; i < texture_size; ++i)
        {
            for (auto j = 0; j < texture_size; ++j)
            {
                for (auto k = 0; k < texture_size; ++k)
                {
                    //(GLubyte)(((noise3(ni)+1.0) * amp) * 128.0);

                    auto sample = noise.GetNoise(i * inc, j * inc, k * inc);

                    *(ptr + f) = static_cast<GLubyte>((sample + 1.0f) * amp * 128.0f);
                    // *(ptr + f) = static_cast<GLubyte>(glm::perlin(ni / frequency) * amp * 128.0f);
                    // ni.z += inc;
                    ptr += 4;
                }
                // ni.y += inc;
            }
            // ni.x += inc;
        }


        frequency *= 2;
        ++inc;
        amp *= 0.5;
    }




    // initialize texture and sampler

//    glActiveTexture(4);
//    std::cerr << "texture is active" << std::endl;

    GLuint texture;
//    glGenTextures(GL_TEXTURE_3D, 1, &texture);
    glGenTextures(1, &texture);
//    std::cerr << "texture is created" << std::endl;
    glActiveTexture(GL_TEXTURE4);
//    std::cerr << "texture is active" << std::endl;
    glBindTexture(GL_TEXTURE_3D, texture);
//    std::cerr << "texture is bound" << std::endl;

    auto number_of_levels = 1;
    auto internal_format = GL_RGBA8;
    auto external_format = GL_RGBA;


//    glTextureStorage3D(texture, number_of_levels, internal_format, texture_size, texture_size, texture_size);
//    glTextureSubImage3D(texture, 0, 0, 0, 0, texture_size, texture_size, texture_size,
//        external_format, GL_UNSIGNED_BYTE, texture_data);
    glTexImage3D(GL_TEXTURE_3D,
                 0, // level
                 internal_format,
                 texture_size, texture_size, texture_size,
                 0, // border
                 external_format,
                 GL_UNSIGNED_BYTE,
                 texture_data
    );

    delete texture_data;
//    glBindTextureUnit(4, texture);

    glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, cs4722::x11::aquamarine.as_float().get());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // auto mag_filter = GL_NEAREST;
    auto mag_filter = GL_LINEAR;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, mag_filter);
    // auto wrap_type = GL_CLAMP_TO_EDGE;
    // auto wrap_type = GL_MIRRORED_REPEAT;
    auto wrap_type = GL_REPEAT;
    // auto wrap_type = GL_CLAMP_TO_BORDER;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap_type);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap_type);


    auto sampler_loc = glGetUniformLocation(program, "Noise");

    glUniform1i(sampler_loc, 4);

}


//----------------------------------------------------------------------------
//
// display


void
display()
{
    // static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
    glClear(GL_DEPTH_BUFFER_BIT);
    // glBindVertexArray(VAOs[0]);

    // vertex shader
    const auto mv_transform_loc = glGetUniformLocation(program, "MVMatrix");
    //const auto mvp_transform = glGetUniformLocation(the_scene->program, " MVPMatrix");
    const auto n_transform = glGetUniformLocation(program, "NormalMatrix");
    const auto light_position = glGetUniformLocation(program, "LightPos");
    const auto cloud_scale = glGetUniformLocation(program, "Scale");



    // const auto light_color = glGetUniformLocation(the_scene->program, "LightColor");
    // const auto u_shininess = glGetUniformLocation(the_scene->program, "shininess");
    // const auto strength = glGetUniformLocation(the_scene->program, "strength");
    const auto SkyColor = glGetUniformLocation(program, "SkyColor");
    const auto CloudColor = glGetUniformLocation(program, "CloudColor");
    // const auto u_material_color = glGetUniformLocation(the_scene->program, "u_material_color");
    // const auto u_ambient = glGetUniformLocation(the_scene->program, "u_ambient");


    // glUniform4fv(u_ambient, 1, the_scene->the_light.ambient_light.as_float());
    // glUniform4fv(light_color, 1, the_scene->the_light.light_color.as_float());
    glUniform4fv(light_position, 1, glm::value_ptr(the_light.light_direction_position));


    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect, the_view->perspective_near);
    auto vp_transform = projection_transform * view_transform;

    auto p_transform = glGetUniformLocation(program, "p_transform");
    glUniformMatrix4fv(p_transform, 1, GL_FALSE,
                       glm::value_ptr(projection_transform));

    // auto vp_transform = projection_transform * view_transform;



    static auto last_time = 0.0;
    auto time = glfwGetTime();
    auto delta_time = time - last_time;

    for (auto *artf: artifact_list) {

        artf->animate(time, delta_time);

        auto model_transform = artf->animation_transform.matrix() * artf->world_transform.matrix();

        // glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(transform));
        auto mv_transform = view_transform * model_transform;
        glUniformMatrix4fv(mv_transform_loc, 1, GL_FALSE, glm::value_ptr(mv_transform));
        // glUniformMatrix4fv(mvp_transform, 1, GL_FALSE,
        // 	glm::value_ptr(projection_transform * mv_transform));

        // glUniform1i(uSampler, artf->texture_unit());
        glUniform4fv(SkyColor, 1, artf->surface_material.diffuse_color.as_float().get());
        glUniform4fv(CloudColor, 1, artf->surface_material.ambient_color.as_float().get());
        glUniformMatrix4fv(n_transform, 1, GL_FALSE,
                           glm::value_ptr(glm::inverseTranspose(mv_transform)));
        // glUniform1f(u_shininess, artf->shininess);
        // std::cout << "shininness for artifact " << artf->shininess() << std::endl;
        // glUniform1f(strength, artf->specular_strength);

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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Clouds", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

	init();
    the_view->perspective_aspect = aspect_ratio;

//	init_texture3D();
    init_texture3D_F();

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
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
}
