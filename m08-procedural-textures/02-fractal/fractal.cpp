


//#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

//#define _USE_MATH_DEFINES
//#include <cmath>
#include <iostream>

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <vector>

#include "learnopengl/shader.h"

#include "cs4722/x11.h"


const auto  number_of_vertices = 6;

GLuint program;

void
init1(void)
{

    Shader shader("vertex_shader02.glsl",
                  "fragment_shader02.glsl");
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_PROGRAM_POINT_SIZE);


    GLfloat a = 1;
    glm::vec4 ul = glm::vec4(-a, a, 0, 1);
    glm::vec4 ll = glm::vec4(-a, -a, 0, 1);
    glm::vec4 lr = glm::vec4(a, -a, 0, 1);
    glm::vec4 ur = glm::vec4(a, a, 0, 1);

    glm::vec4 vertices[number_of_vertices] = { ll, ur, ul, ur, ll, lr };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
	
    const auto number_of_buffers = 1;
	
    GLuint  buffers[number_of_buffers];
    const auto b_position = glGetAttribLocation(program, "bPosition");
    // const auto b_color = glGetAttribLocation(program, "bColor");
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//    glBufferStorage(GL_ARRAY_BUFFER, number_of_vertices*4*4, vertices, 0);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, vertices,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);


}

//----------------------------------------------------------------------------
//
// display
//

static glm::vec2 center = glm::vec2(0, 0);//  glm::vec2(-.5, -.5);
static glm::vec2 range = glm::vec2(1.0, 1.0);

void
display(void)
{
    glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());

    auto center_loc = glGetUniformLocation(program, "center");
    auto range_loc = glGetUniformLocation(program, "range");
    glUniform2fv(center_loc, 1, glm::value_ptr(center));
    glUniform2fv(range_loc, 1, glm::value_ptr(range));

    glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
}



void general_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const auto scaleSlide = .1f;
    const auto scaleZoom = .9f;
	
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
	    switch (key)
	    {
        case GLFW_KEY_LEFT:
            center.x -= scaleSlide * range.x;
	    	break;
        case GLFW_KEY_RIGHT:
            center.x += scaleSlide * range.x;
            break;
        case GLFW_KEY_UP:
            center.y += scaleSlide * range.y;
            break;
        case GLFW_KEY_DOWN:
            center.y -= scaleSlide * range.y;
            break;
        case GLFW_KEY_S:
            range /= scaleZoom;
            break;
        case GLFW_KEY_W:
            range *= scaleZoom;
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
	    }

        printf("center(%g, %g)  range(%g, %g)\n", center.x, center.y,
        range.x, range.y);
	    
    }
    else
    {


    }


}



int
main(int argc, char** argv)
{
    glfwInit();
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


    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Square", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    init1();

    std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL renderer " << glGetString(GL_RENDERER) << std::endl;

    auto iteration_limit_loc = glGetUniformLocation(program, "iteration_limit");
    glUniform1i(iteration_limit_loc, 300);

    auto colors_loc = glGetUniformLocation(program, "colors");
    auto num_colors_loc = glGetUniformLocation(program, "num_colors");

	auto* colors = new std::vector<cs4722::color>({
    cs4722::x11::navajo_white1, cs4722::x11::navajo_white2, cs4722::x11::navajo_white3, cs4722::x11::navajo_white4,
    cs4722::x11::sky_blue1, cs4722::x11::sky_blue2, cs4722::x11::sky_blue3, cs4722::x11::sky_blue4,
    cs4722::x11::orange1,cs4722::x11::orange2,cs4722::x11::orange3,cs4722::x11::orange4,
    cs4722::x11::green1,cs4722::x11::green2,cs4722::x11::green3,cs4722::x11::green4,
        });
    auto* colors_fl = new std::vector<float>();
    for (auto i = colors->begin(); i != colors->end(); ++i)
    {
        float* cf = i->as_float().get();
        for (int j = 0; j < 4; j++)
        {
            colors_fl->push_back(cf[j]);
        }
    }
    glUniform4fv(colors_loc, colors->size(), colors_fl->data());
    glUniform1i(num_colors_loc, colors->size());

    glfwSetKeyCallback(window, general_key_callback);


    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
