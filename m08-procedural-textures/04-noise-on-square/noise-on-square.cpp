


#include <GLM/vec4.hpp>
#include <GLM/vec2.hpp>
#include <GLM/vec3.hpp>
#include <glm/gtc/noise.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/x11.h"
//#include "../callbacks.h"


#include "FastNoiseLite.h"


const GLuint  NumVertices = 6;
GLuint program;

void init(void)
{

    Shader shader("vertex_shader04.glsl",
                  "fragment_shader04.glsl");
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_PROGRAM_POINT_SIZE);
}

void init_buffers() {


    GLfloat a = .7f;
    GLfloat b = .7f;
    glm::vec4 ul = glm::vec4(-b, a, 0, 1);
    glm::vec4 ll = glm::vec4(-b, -b, 0, 1);
    glm::vec4 lr = glm::vec4(a, -b, 0, 1);
    glm::vec4 ur = glm::vec4(a, a, 0, 1);

    glm::vec4 position_list[NumVertices] = { ll, ur, ul, ur, ll, lr };


    // generate the texture coordinate data

	auto upper_texture = 1.0f;
	auto lower_texture = 0.0f;
	auto ult = glm::vec2(lower_texture, upper_texture);
	auto llt = glm::vec2(lower_texture, lower_texture);
	auto lrt = glm::vec2(upper_texture, lower_texture);
	auto urt = glm::vec2(upper_texture, upper_texture);
	glm::vec2 texture_coordinate_list[NumVertices] = { llt, urt, ult, urt, llt, lrt };


    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const auto number_of_buffers = 2;
    GLuint buffers[number_of_buffers];

    glGenBuffers(number_of_buffers, buffers);
    auto bPosition = glGetAttribLocation(program, "bPosition");
    auto bTextureCoord = glGetAttribLocation(program, "bTextureCoord");
   
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//    glBufferStorage(GL_ARRAY_BUFFER, 4 * 4 * NumVertices, position_list, 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * NumVertices, position_list,  GL_STATIC_DRAW);
    glVertexAttribPointer(bPosition, 4, GL_FLOAT,
        GL_FALSE, 0, 0);
    glEnableVertexAttribArray(bPosition);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
//    glBufferStorage(GL_ARRAY_BUFFER, 4 * 2 * NumVertices,
//        texture_coordinate_list, 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * NumVertices, texture_coordinate_list,  GL_STATIC_DRAW);
    glVertexAttribPointer(bTextureCoord, 2, GL_FLOAT,
        GL_FALSE, 0, 0);
    glEnableVertexAttribArray(bTextureCoord);

   

}


void init_texture3D() {


    const auto texture_size = 8;
    auto *texture_data = new GLubyte[4*texture_size*texture_size*texture_size];

    auto const number_of_octaves = 4;

	auto  frequency = 4.0f;
    auto amp = 0.5f;

	
	for(auto f = 0; f < number_of_octaves; ++f)
	{
        auto inc = 1.0f / (texture_size / frequency);

        auto* ptr = texture_data;
		
        for (auto i = 0; i < texture_size; ++i)
        {
	        for (auto j = 0; j < texture_size; ++j)
	        {
		        for (auto k = 0; k < texture_size; ++k)
		        {

                    *(ptr + f) = static_cast<GLubyte>((glm::simplex(glm::vec3(i, j,k) * inc) + 1.0f) * amp * 128.0f);
                    ptr += 4;
		        }
	        }
        }

		
        frequency *= 2;
        ++inc;
        amp *= 0.5;
	}


	

    // initialize texture and sampler

    GLuint texture;
//    glGenTextures(GL_TEXTURE_3D, 1, &texture);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_3D, texture);

    auto number_of_levels = 1;
    auto internal_format = GL_RGBA8;
    auto external_format = GL_RGBA;

   
//    glTextureStorage3D(texture, number_of_levels, internal_format, texture_size, texture_size, texture_size);
//    glTextureSubImage3D(texture, 0, 0, 0, 0, texture_size, texture_size, texture_size,
//       external_format, GL_UNSIGNED_BYTE, texture_data);

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
//    glBindTextureUnit(3, texture);

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
    

    auto sampler_loc = glGetUniformLocation(program, "sampler");

    glUniform1i(sampler_loc, 3);
    
}



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


    auto sampler_loc = glGetUniformLocation(program, "sampler");

    glUniform1i(sampler_loc, 4);

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
                *(ptr + f) = static_cast<GLubyte>((glm::simplex(glm::vec2(i,j) * inc) + 1.0f) * amp * 128.0f);
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

    GLuint texture;
//    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);

    auto number_of_levels = 1;
    auto internal_format = GL_RGBA8;
    auto external_format = GL_RGBA;


//    glTextureStorage2D(texture, number_of_levels, internal_format, texture_size, texture_size);
//    glTextureSubImage2D(texture, 0, 0, 0, texture_size, texture_size,
//        external_format, GL_UNSIGNED_BYTE, texture_data);
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
//    glBindTextureUnit(2, texture);

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


    auto sampler_loc = glGetUniformLocation(program, "sampler2");

    glUniform1i(sampler_loc, 2);

}




//----------------------------------------------------------------------------
//
// display
//

void
display(void)
{
    glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

int
main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Multiple Textures", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);
//    cs4722::setup_debug_callbacks();


    init();
    init_buffers();
//     init_texture3D();
//     init_texture2D();
    init_texture3D_F();
	
//    std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
//    std::cout << "OpenGL renderer " << glGetString(GL_RENDERER) << std::endl;
//    GLint max_texture_units;
//    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
//    std::cout << "max texture units " << max_texture_units << std::endl;
//    GLint max_texture_size;
//    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
//    std::cout << "max texture size " << max_texture_size << std::endl;
	

    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
