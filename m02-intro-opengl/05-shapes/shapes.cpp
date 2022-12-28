


/*
 * The main change from the previous example is in init().
 * The model is set up using subclasses of class shape.
 * The buffers are initialized using data obtained from the shapes.
 *
 * shape objects represent the geometry of various shapes such as spheres and cubes.
 * The shapes are approximated by a collection of triangles. (triangulation)
 *      For simplicity, these will always be drawn
 *      using triangles mode in our examples.
 *      However, the other drawing modes have been left in the display function if you want to experiment.
 * Some shape subclasses have parameters that adjust the triangulation.
 *      More triangles gives a smoother approximation, but takes more space and rendering time.
 *
 *
 *  One other small but important change is the addition of depth testing.
 *  This is enabled near the beginning of function init().
 */

#include <GLM/glm.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "cs4722/x11.h"
#include "cs4722/shape.h"
#include "learnopengl/shader.h"


static int  number_of_vertices;

const auto b_position = 1;
const auto b_color = 2;

void init(void)
{

    /*
     * Using the same shaders as in the 'many colors' example.
     */
    Shader shader("vertex_shader03.glsl","fragment_shader03.glsl");
    const auto program = shader.ID;
    shader.use();

    glPointSize(15);

    /*
     *  Depth testing affects how the fragment shader colors a pixel when two fragments are at the same pixel position.
     *      When depth testing is in force, the color of the fragment that is nearer to the
     *      viewer (smaller z coordinate) will be used.
     *  If depth testing is not enabled, then each fragment color is placed into the corresponding pixel.  So,
     *      the color of the last fragment drawn on a particular pixel is the one used for that pixel.
     *      This is called the painter's algorithm.  (watch a Bob Ross video to find out why)
     *  Try commenting out this line to see the effect.
    */
    glEnable(GL_DEPTH_TEST);


    /*
     * The shapes we will be using in this class are: sphere, block, cylinder, and torus.
     * Each shape has a standard position, orientation, and size.
     * For example, the sphere is centered at the origin and is of unit radius.
     *
     * Please check the API to interpret the parameters.
     *
     * Uncomment one line at a time to see what that shape looks like.
     *
     */
     cs4722::shape* shape = new cs4722::sphere(5, 10);
//     cs4722::shape* shape = new cs4722::sphere(5, 22);
//     cs4722::shape* shape = new cs4722::sphere(50, 220);
//     cs4722::shape* shape = new cs4722::torus(.1, 15, 30);
//        cs4722::shape* shape = new cs4722::torus(.5, 60, 125);
//    cs4722::shape* shape = new cs4722::cylinder(1, 20);
//    cs4722::shape* shape = new cs4722::cylinder(1, 200);
    // cs4722::shape* shape = new cs4722::block();

    /*
     * Uncomment the following line to see alternate colors
     */
//    shape->color_set_ = std::vector<cs4722::color>({cs4722::x11::wheat3, cs4722::x11::wheat});

    /*
     * The get_size() method for a shape returns the number of vertices that the shape
     * uses.
     */
	std::cout << "number of vertices " << shape->get_size() << std::endl;


	/*
	 * Setting number_of_vertices so it can be used in the display function below.
	 */
    number_of_vertices = shape->get_size();

    /*
     * Each shape returns a 'vector' of positions describing the locations of the vertices for the triangulation.
     * Positions will be repeated in different vertices when several triangles touch the same vertex.
     *
     * The vector has been dynamically allocated by the positions() method.  Ownership is passed to us,
     *      so we must manage the storage.
     *      At the end of the init() function the vector positionsV is deallocated.
     */
    std::vector<glm::vec4>* positionsV = shape->positions();
    /*
     * A vector cannot be used directly in the buffer functions for OpenGL.
     * The data() method of a std::vector returns a pointer to the data.
     * The data is adjacent in memory since the data is stored in an array.
     * This is suitable then for the OpenGL functions.
     */
    glm::vec4* positions = positionsV->data();
   

    /*
     * Each shape defines a default color scheme in which the triangles are solid color.
     * As much as possible, the colors alternate.  However, this cannot be implemented in all cases.
     *
     * Most shapes use a pair of colors that have default values.
     * See the API for how to change the colors used.
     * The block is an exception, it uses a color list with six colors.  Each side is a different color.
     */
    std::vector<cs4722::color>* colorsV = shape->colors();
    /*
     * Get a pointer to the actual data, a series of bytes.
     */
    cs4722::color* colors = colorsV->data();

    // Generate and bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);




    const auto number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * number_of_vertices, positions, GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * number_of_vertices, colors,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);
    glEnableVertexAttribArray(b_color);

    /*
     * We don't need these vectors anymore, so they are deleted.
     */
    delete positionsV;
    delete colorsV;
}




//----------------------------------------------------------------------------
//
// display
//

void
display(void)
{
//    glDrawArrays(GL_POINTS, 0, number_of_vertices);
//    glDrawArrays(GL_LINES, 0, number_of_vertices);
//    glDrawArrays(GL_LINE_STRIP, 0, number_of_vertices);
//    glDrawArrays(GL_LINE_LOOP, 0, number_of_vertices);
    glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, number_of_vertices);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, number_of_vertices);
}

int
main(int argc, char** argv)
{
    glfwInit();

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


    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Shapes", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    init();

    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float().get());


        /*
        * Each time we render the scene we must clear the depth buffer.
        * The depth buffer records the distance to the nearest fragment that covers a particular pixel.
        * If another fragment is processed later that covers the same pixel, its color is used only
        *      if it is nearer than the previous fragment.
        *      If the new fragment is closer, then the distance to that nearer fragment is placed in the
        *      depth buffer.
        *
        *  It is important that the depth buffer be cleared before rendering, otherwise values from a previous
        *      rendering could confuse the algorithm.
        */
        glClear(GL_DEPTH_BUFFER_BIT);
        
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
