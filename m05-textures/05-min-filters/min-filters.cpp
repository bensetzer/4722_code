


#include <GLM/vec4.hpp>
#include <GLM/vec2.hpp>
#include <GLM/gtc/type_ptr.hpp>


#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/view.h"
#include "cs4722/artifact.h"
#include "../callbacks.h"


static cs4722::view *the_view;
static std::vector<cs4722::artifact*> artifact_list;
static GLuint program;
static std::vector<GLuint> texture_list;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_texture_coords = 2;

const static auto u_sampler = 2;



void init()
{
    the_view = new cs4722::view();
    the_view->set_camera_position(glm::vec3(1.5, 1.6, 1.7));


    Shader shader("vertex_shader3.glsl","fragment_shader3.glsl");
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);


    auto r_height = 10.0;
    auto r_width = 3.0;
    auto *a_shape = new cs4722::rectangle;
    a_shape->texture_height_scale = (r_height);
    a_shape->texture_width_scale = (r_width);
    auto *artf = new cs4722::artifact();
    artf->the_shape = (a_shape);
    artf->world_transform.rotation_axis = (glm::vec3(1, 0, 0));
    artf->world_transform.rotation_angle = (-M_PI / 2);
    artf->world_transform.scale = (glm::vec3(r_width, r_height, 1));
    artf->texture_unit = 1;

    artifact_list.push_back(artf);


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

void init_texture_computed() {

    auto texture_width = 2048;
    auto texture_height = texture_width;
    auto size = texture_width / 8;
	
    auto texture_data = new std::vector<GLubyte>();
    // create data
    for (int r = 0; r < texture_height; r++) {
        for (int c = 0; c < texture_width; c++) {
            auto i = r / size;
            auto j = c / size;
        	if((i+j) % 2 == 0)
        	{
                texture_data->push_back(0);
                texture_data->push_back(0);
                texture_data->push_back(0);
        	} else
        	{
                texture_data->push_back(255);
                texture_data->push_back(255);
                texture_data->push_back(255);

        	}
            texture_data->push_back(255);
        }
    }


    // initialize texture and sampler

    glActiveTexture(GL_TEXTURE1);
    GLuint texture;
    glGenTextures(1, &texture);
    texture_list.push_back(texture);
    glBindTexture(GL_TEXTURE_2D, texture);

//    std::cout << "texture in create texture: " << texture << std::endl;

    auto number_of_levels = 10;
    auto internal_format = GL_RGBA8;
    auto external_format = GL_RGBA;

   
//    glTexStorage2D(GL_TEXTURE_2D, number_of_levels, internal_format, texture_width, texture_height);
////    glBindTexUnit(1, texture);
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_width, texture_height,
//       external_format, GL_UNSIGNED_BYTE, texture_data->data());
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, texture_width, texture_height, 0, external_format,
                 GL_UNSIGNED_BYTE, texture_data->data());

    // glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, cs4722::x11::aquamarine.as_float().get());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    auto mag_filter = GL_NEAREST;
    // auto mag_filter = GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    // auto wrap_type = GL_CLAMP_TO_EDGE;
    //auto wrap_type = GL_MIRRORED_REPEAT;
    auto wrap_type = GL_REPEAT;
    //auto wrap_type = GL_CLAMP_TO_BORDER;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_type);
    glGenerateMipmap(GL_TEXTURE_2D);
}



void init_texture_computed_from_text() {

    glActiveTexture(GL_TEXTURE0);
    GLuint tex;
    glGenTextures(1, &tex);
    std::cout << "texture in create texture 2: " << tex << std::endl;
    texture_list.push_back(tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 7, GL_RGBA8, 64, 64);

    unsigned int* data = new unsigned int[64 * 64];

    unsigned int colors[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF };

    int i, j, k;
    int n;

    for (i = 0; i < 7; i++)
    {
        n = 0;
        for (j = 0; j < (64 >> i); j++)
        {
            for (k = 0; k < (64 >> i); k++)
            {
                data[n] = (k ^ (64 - j)) * 0x04040404;
                n++;
            }
        }
        glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, 64 >> i, 64 >> i, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 4.5f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    delete[] data;
}

void
display()
{


    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect, the_view->perspective_near);
    auto vp_transform = projection_transform * view_transform;



    for (auto artf : artifact_list) {
        auto model_transform =  artf->animation_transform.matrix() *  artf->world_transform.matrix();
        auto transform = vp_transform * model_transform;
    	glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform1i(u_sampler, artf->texture_unit);
        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start,
            artf->the_shape->buffer_size);
    }
}




//int old_width, old_height, old_xpos, old_ypos;

void local_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
//    auto* the_scene = static_cast<cs4722::scene*>(glfwGetWindowUserPointer(window));
    auto texture_unit = artifact_list[0]->texture_unit;
    auto texture = texture_list[texture_unit];
    glBindTexture(GL_TEXTURE_2D, texture);


	if(action != GLFW_PRESS && action != GLFW_REPEAT)
	{
		
	}
    else if (key == GLFW_KEY_DOWN)
    {
        auto* artf = artifact_list[0];
        artf->world_transform.rotation_angle -=  M_PI / 30;

    }
    else if (key == GLFW_KEY_UP)
    {
        auto* artf = artifact_list[0];
        artf->world_transform.rotation_angle += M_PI / 30;

    }
    else if (key == GLFW_KEY_0)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else if(key == GLFW_KEY_1)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else if (key == GLFW_KEY_5)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else if (key == GLFW_KEY_4)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
    else if (key == GLFW_KEY_3)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    }
    else if (key == GLFW_KEY_2)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    }
    else if(key == GLFW_KEY_T)
    {
        artifact_list[0]->texture_unit = 1 - artifact_list[0]->texture_unit;
    }
    else
    {
        general_key_callback(window, key, scancode, action, mods);
    }

}


int
main(int argc, char** argv)
{

    /*
    up arrow        Raise rectangle
	down arrow      Lower rectangle
	0               nearest
	1               linear
	5               linear linear
	4               linear nearest
	3               nearest linear
	2               nearest nearest
    T               toggle texture
    
     */

	
    std::cout << "Keys to activate example features:" << std::endl;
    std::cout << "up arrow        Raise surface" << std::endl;
    std::cout << "down arrow      Lower surface" << std::endl;
    std::cout << "0               nearest" << std::endl;
    std::cout << "1               linear" << std::endl;
    std::cout << "2               nearest nearest" << std::endl;
    std::cout << "3               nearest linear" << std::endl;
    std::cout << "4               linear nearest" << std::endl;
    std::cout << "5               linear linear" << std::endl;
    std::cout << "T               Toggle texture" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "" << std::endl;

    glfwInit();

    // set the OpenGL version to use: 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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

    init_texture_computed_from_text();
    init_texture_computed();

    glfwSetWindowUserPointer(window, the_view);

    glfwSetKeyCallback(window, local_key_callback);
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
