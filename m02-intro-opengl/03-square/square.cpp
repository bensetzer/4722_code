
/*
    There are two things of note in this example:
    - Two attributes are provided for each vertex. One attribute is positions the other is
        colors
    - We examine the details of how the color makes its way from the buffer to the
        vertex shader to the fragment shader.
        This includes a pair of variables used as the endpoints of a channel to send
        data from the vertex shader to the fragment shader.
        We also consider what happens to that data since several vertices will contribute to 
        the value a single color value for each fragment.
*/


#include <GLM/glm.hpp>
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
    /*
     * Corner positions defined: upper left, lower left, lower right, upper right
        The square is centeredd at the origin.
     */
    glm::vec4 ul = glm::vec4(-a, a, 0, 1);
    glm::vec4 ll = glm::vec4(-a, -a, 0, 1);
    glm::vec4 lr = glm::vec4(a, -a, 0, 1);
    glm::vec4 ur = glm::vec4(a, a, 0, 1);

    /*
     * Set up the list of positions:
     * The first triangle is lower left, upper right, upper left
     * The second triangle is upper right, lower left, lower right
     *
     * Notice that the ll and the ur vertices are used in both triangles.
     * That is typical when using triangles to model a non-triangular surface.
     *
     * Also, the vertices are taken in counter-clockwise order around each triangle.
     * This could be used to indicate that we are looking at the outside face of the triangle.
     *      This makes little difference here but could make rendering more efficient by
     *      leaving out triangles facing away from the viewer.
     */
    glm::vec4 vertices[number_of_vertices] = { ll, ur, ul, ur, ll, lr };


    /*
     * There are four color schemes provided.
     * You can change color scheme by finding the glBufferStorage function call below, just
     *      after a comment that describes how to change the color scheme.
     */

	/*
	 * This color scheme gives all vertices the same color.
	 * The square is then colored in that one color.
	 */
    const cs4722::color colors1[number_of_vertices] = { cs4722::x11::orange,
      cs4722::x11::orange,cs4722::x11::orange,cs4722::x11::orange,cs4722::x11::orange,
      cs4722::x11::orange,
    };

    /*
     * Each triangle is a solid color but the two triangles have different colors.
     *
     * Notice that ll and ur get assigned different colors depending on which vertex is using them.
     * Colors are assigned to vertices, not to positions..
     */
    const cs4722::color colors2[number_of_vertices] = { cs4722::x11::blue,
                        cs4722::x11::blue, cs4722::x11::blue, cs4722::x11::yellow,
                        cs4722::x11::yellow, cs4722::x11::yellow,
                        };
    // { ll, ur, ul, ur, ll, lr };

    /*
	 * Similar to the previous scheme, though each triangles shows a gradient since the
	 * vertices of each triangle differ in color from one another.
	 *
     * Notice that ll and ur get assigned different colors depending on which vertex is using them.
     * Colors are assigned to vertices, not to positions..
	 */
    const cs4722::color colors3[number_of_vertices] = { cs4722::x11::white,
      cs4722::x11::blue, cs4722::x11::black, cs4722::x11::yellow,
      cs4722::x11::gray, cs4722::x11::orange,
    };
    // { ll, ur, ul, ur, ll, lr };


	// All corners have consistent colors, but diagonal stripe is still evident
	/*
	 * In this color scheme, each triangle has different colors on each vertex.
	 * However, the colors on vertices at the same position are given the same color.
	 * So, both vertices at the lower left are given the color white.
	 * Both vertices at the upper right are given the color blue.
	 *
	 * In this way the gradient extends smoothly across the whole square.
	 *
	 * Note, however, that the diagonal from lower left to upper right is still evident.
	 * The gradient is define over the triangles separately, not over the whole square.
	 */
    const cs4722::color colors4[number_of_vertices] = { cs4722::x11::white,
     cs4722::x11::blue, cs4722::x11::black, cs4722::x11::blue,
     cs4722::x11::white, cs4722::x11::orange,
   };
    // { ll, ur, ul, ur, ll, lr };


// Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);



    /*
        We'll use two buffers, one for positions and one for colors.
    */
    const auto number_of_buffers = 2;	
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, vertices,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    /*
        Color data uses four bytes for each color.
        One byte is used to represent each color component: red, green, blue, alpha
            alpha is generally opacity/transparency
        So, the number of bytes is computed as 4 * 1 * number_of_vertices.

     * Select the color scheme you want to see here.
     * Change 'colors1' to 'colors2' or 'colors3' or 'colors4'.
     */
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * number_of_vertices, colors1,  GL_STATIC_DRAW);
    /*
        The description of the data layout seems to be pretty straightforward:
        4 components for color per vertex, using unsigned bytes for the data.
        However, if you look in the vertex shader you will see that the bColor variable
        that receives this data is a vec4, that is, four float values.
        Also, the fragment shader assumes that color components are floating values
        in the range from 0 to 1.

        Notice, however, that the fourth parameter is GL_TRUE.
        This tells OpenGL to normalize the data when handing it over to the vertex
        shader.
        This means dividing the input values (integers in the range 0 to 255) by
        the maximum possible value (255) getting float values in the range
        from 0 to 1.
        This is referred to as 'normalization'.
        The normalized values are what end up in bColor when the vertex shader executes.
    */
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(b_color);


}

//----------------------------------------------------------------------------
//
// display
//

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
