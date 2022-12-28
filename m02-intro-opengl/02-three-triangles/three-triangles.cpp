// comments done
// code update done


/*
 * The purpose of this example is to show a way of working with multiple shapes that each have
 * different associated uniform data.
 * The example is to draw three triangles in three different positions and three different
 * colors.
 * There are many approaches to this but we will follow this strategy:
 *      Draw each triangle with a separate glDrawArrays function call and set the color uniform
 *      variable to the proper value just before calling glDrawArrays.
 * We will use one buffer for all the vertex position data, so we will have to figure out
 *      how to place the data into the buffer and then how to reference the
 *      data when drawing.
 *
 */

/*
 *   As we move from example to example we will not usually repeat comments from previous examples
 *   for a part of the program that has not changed.
 */


#include <iostream>

#include <glad/gl.h>

#include <GLM/glm.hpp>
#include <GLFW/glfw3.h>

#include "learnopengl/shader.h"

#include "cs4722/x11.h"

const auto  number_of_vertices = 3;

const unsigned int color_loc = 1;

const auto b_position = 1;

static const auto background_color = cs4722::x11::gray70;

/*
 * The following 32 lines of code define three shapes and the information needed to
 * access the vertex positions that define these shapes.
 * We would normally not want to include so much in the global part of the program code
 * but many values have to be shared with the render() function.
 */
/*
 * The positions for the vertices of a triangle are set up here just as was done
 * in the previous example.
 */
glm::vec4 positions0[] = {
        glm::vec4(-.5, .8, 0, 1),
        glm::vec4(-.8, .2,0, 1),
        glm::vec4(-.2, .2, 0, 1)
};
auto num_vertices0 = 3;
/*
 * How many bytes is used to store this vertex position data?
 */
auto size0 = 4 * 4 * num_vertices0;
/*
 * Imagine the vertices for the three triangles packed together.
 * At what vertex index will the vertices for this shape start?
 * Since it's  first, we'll start it at offset equal to 0.
 */
auto offset0 = 0;
/*
 * Imagine all the position data for the three triangles packed into the buffer.
 * At what byte position will the data for this shape start?
 * Since it's first, we'll start it at offset equal to 0.
 *
 * bOffset  is for byte offset
 */
auto bOffset0 = 0;
/*
 * What color will this shape use?
 */
auto color0 = cs4722::x11::medium_violet_red;

/*
 * Positions for the second shape.
 */
glm::vec4 positions1[] = {
        glm::vec4(.5, .8, 0, 1),
        glm::vec4(.2, .2,0, 1),
        glm::vec4(.8, .2, 0, 1)
};
auto num_vertices1 = 3;
auto size1 = 4 * 4 * num_vertices1;
/*
    The start vertex index is going to be the start of the previous
    set of vertices, at offset0, plus the number of vertices in
    the previous shape, num_vertices0.
*/
auto offset1 = offset0 + num_vertices0;
/*
    Similarly, the byte offset is the byte offset of the previous shape
    plus the size in bytes of the previous shape.
*/
auto bOffset1 = bOffset0 + size0;
auto color1 = cs4722::x11::slate_blue;

glm::vec4 positions2[] = {
        glm::vec4(.5, -.2, 0, 1),
        glm::vec4(.2, -.8,0, 1),
        glm::vec4(.8, -.8, 0, 1)
};
auto num_vertices2 = 3;
auto size2 = 4 * 4 * num_vertices2;
auto offset2 = offset1 + num_vertices1;
auto bOffset2 = bOffset1 + size1;
auto color2 = cs4722::x11::goldenrod1;


void init(void)
{
    /*
     * Compile and use the shaders.
     */
    Shader shader("vertex_shader02.glsl","fragment_shader02.glsl");
    auto program = shader.ID;
    if(program < 0) {
        printf("Error in shader compilation");
        std::exit(-1);
    }
    shader.use();


    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    const auto number_of_buffers = 1;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    /*
        The buffer size computation is really the same as the previous example except
        we must count up the vertices of all three shapes.

        Also, we don't have our data all packed together.
        So, we start by reserving the area but put no data in the buffer.
        The third parameter is nullptr to indicate that there is not data provided yet.
    */
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * (num_vertices0 + num_vertices1 + num_vertices2),
                 nullptr,  GL_STATIC_DRAW);
    /*
        Put the vertices for the first shape into the bufffer.
        The second parameter specifies the byte offset at which to place the data.
        The third parameter specifies the number of bytes.
        The fourth parameter points to the data in memory.
    */
    glBufferSubData(GL_ARRAY_BUFFER, bOffset0, size0, positions0);
    glBufferSubData(GL_ARRAY_BUFFER, bOffset1, size1, positions1);
    glBufferSubData(GL_ARRAY_BUFFER, bOffset2, size2, positions2);

    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(b_position);
}


void render(void)
{

    /*
        Set the color for the first shape
    */
    glUniform4fv(color_loc, 1, color0.as_float().get());
    /*
        Draw the first shape.
        the second parameter specifies the offset, in vertices, where the vertex data
            begins in the buffer.
        The third parameter specifies how many vertices will be used for that drawing operation.
    */
    glDrawArrays(GL_TRIANGLES, offset0, num_vertices0);

    glUniform4fv(color_loc, 1, color1.as_float().get());
    glDrawArrays(GL_TRIANGLES, offset1, num_vertices1);
    glUniform4fv(color_loc, 1, color2.as_float().get());
    glDrawArrays(GL_TRIANGLES, offset2, num_vertices2);
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    auto* primary = glfwGetPrimaryMonitor();
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
    std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
    const auto size = glm::min(width, height);
    const auto ratio = .9f;
    const auto w_width = static_cast<int>(ratio * size);
    const auto w_height = w_width;
    const auto w_x = static_cast<int>(size * (1 - ratio) / 2);
    const auto w_y = w_x;
    auto* window = glfwCreateWindow(w_width, w_height, "Three Triangles", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    init();

    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, background_color.as_float().get());
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
