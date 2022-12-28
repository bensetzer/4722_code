
/*
    This example is the same as the square example except that a single buffer
    is used for both the colors and the positions.

    According to one discussion it is more efficient, at least for the GPU, to
    organize buffers this way.
    This does require grubbing around counting bytes.

    the layout will look like this:

    position  color  position color  position color  position color

    the first position will be at byte offset 0
    The first color will be at byte offset 16
        16 is the number of bytes in the first position added to its offset
    The second position will be at byte offset 20
        20 is the number of bytes in a color, 4, added to the offset of the first color, 16
    The second color will be at byte offset 36
    ...

    The number of bytes from the start of one position to the start of the next position
        is 20 bytes.
        This is called the stride.
    The stride is the same for the colors.


    We will not use this approach in the remainder of our examples, it adds complexity while not 
    advancing the course objectives.
    If we were using an ECS framework (Entity Control System) then the one buffer approach
    would be easier because these frameworks typically store data in a way that matches
    the buffer layout.
    So, the transfer of data from CPU memory to GPU buffer is much easier than in this
    example.


*/


#include <GLM/glm.hpp>
#include "GLM/gtc/type_ptr.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "cs4722/x11.h"
#include "learnopengl/shader.h"

static const auto  number_of_vertices = 6;

static const auto b_position = 1;
static const auto b_color = 2;


void init(void)
{

    /*
     * Using the same shaders as in the previous example.
     */
    Shader shader("vertex_shader03.glsl","fragment_shader03.glsl");
    const auto program = shader.ID;
    shader.use();

    glEnable(GL_PROGRAM_POINT_SIZE);


    /*
     * model a square from two triangles.
     */

    /*
     *  The value a is half the length of the side of the square.
     */
    GLfloat a = .5;
    glm::vec4 ul = glm::vec4(-a, a, 0, 1);
    glm::vec4 ll = glm::vec4(-a, -a, 0, 1);
    glm::vec4 lr = glm::vec4(a, -a, 0, 1);
    glm::vec4 ur = glm::vec4(a, a, 0, 1);
    glm::vec4 positions[number_of_vertices] = { ll, ur, ul, ur, ll, lr };


    const cs4722::color colors1[number_of_vertices] = { cs4722::x11::orange,
      cs4722::x11::orange,cs4722::x11::orange,cs4722::x11::orange,cs4722::x11::orange,
      cs4722::x11::orange,
    };
    const cs4722::color colors2[number_of_vertices] = { cs4722::x11::blue,
                        cs4722::x11::blue, cs4722::x11::blue, cs4722::x11::yellow,
                        cs4722::x11::yellow, cs4722::x11::yellow,
                        };
    const cs4722::color colors3[number_of_vertices] = { cs4722::x11::white,
      cs4722::x11::blue, cs4722::x11::black, cs4722::x11::yellow,
      cs4722::x11::gray, cs4722::x11::orange,
    };
    const cs4722::color colors4[number_of_vertices] = { cs4722::x11::white,
     cs4722::x11::blue, cs4722::x11::black, cs4722::x11::blue,
     cs4722::x11::white, cs4722::x11::orange,
   };
    // { ll, ur, ul, ur, ll, lr };


    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);



    /*
        We'll use one buffer, shared by positions and colors.
    */
    const auto number_of_buffers = 1;	
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    /*
        The size of the buffer is large enough to hold both the positions
        and the colors.

        No data is provided here, the space is just allocated.
    */
    glBufferData(GL_ARRAY_BUFFER, 
        4 * 4 * number_of_vertices + 4 * 1 * number_of_vertices,
        nullptr,  GL_STATIC_DRAW);

    /*
        See the comments at the top of the file for a discussion of stride.
    */
    auto stride = 4 * 4 + 4 * 1;

    /*
        The first position data is placed starting at offset 20*0
        The position is 16 bytes long
        The second position is placed starting at 20 * 1
        and so n
    */
    for(int i = 0; i < number_of_vertices; i++ ) {
         glBufferSubData(GL_ARRAY_BUFFER, stride*i, 4*4, glm::value_ptr(positions[i]));
    }
    /*
        The only difference in describing the data is specifying the
        stride, the fifth parameter.
        The position data starts at the beginning of the buffer, the last parameter is 0.
        But, instead of each position starting every 16 bytes they are starting every 20
        bytes.
    */
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(b_position);

    /*
        The first color is stored right after the first position so at
            20*0 + 16.
        The second color is stored right aftre the second position so at
            20*1 + 16
        And so on

        &colors4[i] is a pointer to the start of the vector data at index i
        colors4[i] would be the data itself, not what glBufferSubData is expecting.
    */
    for(int i = 0; i < number_of_vertices; i++ ) {
         glBufferSubData(GL_ARRAY_BUFFER, stride*i+16, 4*1, &colors4[i]);
    }
    /*
        The first color begins right after the first position at byte offset
            16.
            The (void *) cast asks the compiler to interpret the 16 as a byte offset.
        As with the positions, the stride parameter is 20.
    */
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)16);
    glEnableVertexAttribArray(b_color);


}


void
display(void)
{
    glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());
   
    glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int
main(int argc, char** argv)
{
    glfwSetErrorCallback(error_callback);
    /* Initialize the library */
    if (!glfwInit())
        return -1;

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

    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Square", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    init();

    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
