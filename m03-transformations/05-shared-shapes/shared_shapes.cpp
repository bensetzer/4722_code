
/*
 *  We saw in the previous example that artifacts can share shapes.
 *
 *
 *
 *  One shape is shared by all the artifacts.
 *  If the shape data is stored for each artifact, then the total number of vertices used in the buffers is 2,673,000
 *  If the shape data is stored just once then the total number of vertices used in the buffers is 792
 *
 */



#include "GLM/gtc/type_ptr.hpp"

#include <glad/gl.h>

#include "learnopengl/shader.h"

#include "cs4722/artifact.h"


static GLuint program;
static std::vector<cs4722::artifact*> artifact_list;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;




void init(void)
{

    Shader shader("vertex_shader-m02.glsl" ,"fragment_shader-m02.glsl" );
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);


    /*
     * Create a single shape that will be used by all the artifacts.
     */
	auto *sphere = new cs4722::sphere();

	/*
	 * The following is code that will be used here and there in examples when we want to create
	 * a large number of artifacts in a grid arrangement.
	 * The artifacts are arranged in a cube.
	 * The number of artifacts in each of the x, y, and z directions is given by the variable 'number'.
	 * In all, number * number * number artifacts will be created.
	 * In this example you will just be able to see the nearest layer.
	 */
    auto number = 15 ;
    /*
     * The next statements figure out how to place the artifacts so they all fit in the standard view volume without
     * overlapping.
     * If number is changed, the entire scene scales appropriately:
     *      making number smaller causes the artifacts to get bigger and be spaced farther apart
     *      making number larger causes the artifacts to get smaller and be spaced closer together.
     */
    /*
     * d is the distance between adjacent artifacts along the axes.
     */
    auto d = 4.0f / (2 * number + 1);
    /*
     * In this example, the shape is a sphere.
     * The variable radius specifies the radius of each artifact.
     */
    auto radius = d / 4;
    /*
     * base is the value at which each artifact starts.
     * This is the distance from the left/bottom/nearest plane of the view cube.
     * The value is chosen to make the distance from the edge equal on all sides.
     */
    auto base = -1 +  3 * radius;

    /*
     * A triple nested loop goes through the number*number*number possibilities of (x,y,z)
     */
	for(auto x = 0; x < number; ++x)
	{
        for (auto y = 0; y < number; ++y)
        {
            for (auto z = 0; z < number; ++z)
            {
                auto* artf = new cs4722::artifact();
                /*
                 * Set the shape.
                 */
                artf->the_shape = sphere;
                /*
                 * The effect of the translation is to move the artifact to a particular location.
                 * The center of the artifact starts at (0,0,0), defined by the sphere shape
                 * The center is moved to (base+x*d, base+y*d, base+z*d) which is then the center of the
                 *      transformed artifact.
                 */
                artf->world_transform.translate = glm::vec3(base + x * d, base + y * d, base + z * d);
                /*
                 * Scaling can change the size of the artifact.
                 * Since the initial radius of the artifact is 1, defined by the sphere shape,
                 *      scaling by 'radius' in every dimension yields a sphere with radius equal to
                 *      the value of the 'radius' variable
                 */
                artf->world_transform.scale = glm::vec3(radius, radius, radius);
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

    /*
     *  Set up buffers
     */
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT,
                          GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 1 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(b_color);

    for (auto artf : artifact_list) {
        if(!artf->shape_is_shared) {
            auto pos = artf->the_shape->positions();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            pos->data());
            delete pos;

            auto colors = artf->the_shape->colors();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 1 * artf->the_shape->buffer_start,
                            4 * 1 * artf->the_shape->buffer_size,
                            colors->data());
            delete colors;
        } else {
            // shape is shared with another artifact, don't have to do anything here
        }
    }

    /*
     *  The output allows us to see the total vertex count for the shapes.
     *  For number = 15  and with a sphere the count is 2673000 vertices with no sharing
     *      and 792 vertices with sharing.
     *  You can see there is a considerable savings in buffer space if the same shape is utilized
     *      many times.
     *
     *  num = 55 then 131769000 when non-shared  and  792  when shared
     */
    std::cout << "size vBuf " << total_vertex_count << std::endl;



}

//----------------------------------------------------------------------------
//
// display
//

void
display()
{
    for (auto artf : artifact_list) {
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(artf->world_transform.matrix()));
        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start, artf->the_shape->buffer_size);
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Shared Shapes", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    init();

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
