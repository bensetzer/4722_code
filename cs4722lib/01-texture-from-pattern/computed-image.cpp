


/*
 * Texture coordinates are two dimensional, so we include vec2.
 */
#include <GLM/vec2.hpp>

#include <vector>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/x11.h"

#include "cs4722/texture_utilities.h"


const GLuint  NumVertices = 6;
GLuint program;

// locations of variables in the shaders
const static auto b_position = 1;
const static auto b_texture_coord = 2;
const static auto u_sampler = 1;

void init(void)
{

    /*
     * Check the fragment shader code for comments.
     */
    Shader shader("vertex_shader01b.glsl","fragment_shader01b.glsl");
    program = shader.ID;
    glUseProgram(program);

    cs4722::init_texture_computed2(2,128, cs4722::x11::magenta, cs4722::x11::yellow);
    glUniform1i(u_sampler, 2);

    /*
     * We are just using a single square to display textures in the first two examples.
     * This will let us see how texture coordinates actually match up to the vertices.
     */

    // generate the vertex position data
    // simple square with corners .7 of the way to the window corners
    GLfloat a = .7f;
    GLfloat b = .7f;
    glm::vec4 ul = glm::vec4(-b, a, 0, 1);
    glm::vec4 ll = glm::vec4(-b, -b, 0, 1);
    glm::vec4 lr = glm::vec4(a, -b, 0, 1);
    glm::vec4 ur = glm::vec4(a, a, 0, 1);

    glm::vec4 position_list[NumVertices] = { ll, ur, ul, ur, ll, lr };


    // generate the texture coordinate data
    // These are the corners of the unit square with one vertex at the origin.
	auto upper_texture = 1.0f;
	auto lower_texture = 0.0f;
	auto ult = glm::vec2(lower_texture, upper_texture);
	auto llt = glm::vec2(lower_texture, lower_texture);
	auto lrt = glm::vec2(upper_texture, lower_texture);
	auto urt = glm::vec2(upper_texture, upper_texture);
	/*
	 * The next list associates texture coordinates with vertices.
	 * Changing the elements in the list will change how the texture is 'painted' onto the surface.
	 */
	glm::vec2 texture_coordinate_list[NumVertices] = { llt, urt, ult, urt, llt, lrt };

    // Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    const auto number_of_buffers = 2;
    GLuint buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * NumVertices, position_list,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    // Texture coordinates are 2D, so two floats per vertex
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * NumVertices, texture_coordinate_list,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_texture_coord, 2, GL_FLOAT,GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_texture_coord);

}


//----------------------------------------------------------------------------
//
// display
//

void
display(void)
{
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "User Interaction", NULL, NULL);
    int w_x = (width - w_width) / 2;
    int w_y = (height - w_height) / 2;
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    init();


/*
 * The texture utilities in the local library provide a function that defines a computed texture.
 * This is similar to the one above but produces some interesting effects, depending on the colors
 *      used.
 */
//    cs4722::init_texture_computed(2,16, cs4722::x11::cyan, cs4722::x11::magenta);
//    glUniform1i(u_sampler, 2);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);





    /*
     * Display characteristics of the graphics card in supporting textures

     Here is the output on my laptop with Intel graphics selected:
        OpenGL version 4.6.0 - Build 26.20.100.7985
        OpenGL renderer Intel(R) UHD Graphics 630
        max texture units 32
        max texture size 2048

    Here is the output with the graphics card selected:
        OpenGL version 4.6.0 NVIDIA 452.41
        OpenGL renderer GeForce GTX 1050 Ti with Max-Q Design/PCIe/SSE2
        max texture units 32
        max texture size 16384



     */
    std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL renderer " << glGetString(GL_RENDERER) << std::endl;
    GLint max_texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
    std::cout << "max texture units " << max_texture_units << std::endl;
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
    std::cout << "max texture size " << max_texture_size << std::endl;
	

    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
