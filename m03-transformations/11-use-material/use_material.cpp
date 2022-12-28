
/*
 * Each artifact has a 'surface_material' property that contains three color values, besides some other
 *      attributes.
 *  We will make use of the material properties when studying lighting and shading.
 *  But, we can illustrate the basics of using a color attribute of an artifact when rendering.
 *  We start with the animation-many example but don't use the colors from the shape.
 *  A unique color will be assigned to each artifact spanning the RBG gamut.
 *
 *  Using a color from the artifact will make use of uniform variable, similar to the one we are using for
 *      the transform applied to each artifact.
 *  In this case, however, the uniform variable will be in the fragment shader.
 *      Unlike buffers, uniform variables can be in either the fragment shader or the vertex shader
 *
 */

#include <vector>

#include "GLM/gtc/type_ptr.hpp"
#include <glad/gl.h>

#include "learnopengl/shader.h"
#include "cs4722/artifact.h"

static GLuint program;

// locations of variables in the shaders
const static auto u_transform = 1;
const static auto u_diffuse_color = 2;
const static auto b_position = 1;

static std::vector<cs4722::artifact*> artifact_list;

void init(void)
{
    Shader shader("vertex_shader-11.glsl" ,"fragment_shader-11.glsl" );
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);



    auto* shape = new cs4722::torus;
//    auto* shape = new cs4722::sphere;
//    auto* shape = new cs4722::block;
//    auto* shape = new cs4722::cylinder;

    auto number = 5;
    auto d = 4.0f / (2 * number + 1);
    auto radius = d / 4;
    auto base = -1 + 3 * radius;

    for (auto x = 0; x < number; ++x)
    {
        for (auto y = 0; y < number; ++y)
        {
            for (auto z = 0; z < number; ++z)
            {
                auto* artf = new cs4722::artifact_rotating();
                artf->the_shape = shape;
                artf->world_transform.translate = glm::vec3(base + x * d, base + y * d, base + z * d);
                artf->world_transform.scale = glm::vec3(radius, radius, radius);
                artf->animation_transform.rotation_axis = glm::vec3(3 * x + 1, 3 * y + 1, 3 * z + 1);
                artf->animation_transform.rotation_center =
                        artf->world_transform.matrix() * glm::vec4(-1, 0, 0, 1);
                artf->rotation_rate = M_PI / 3 * pow(z+1, .5);

                /*
                 * The formula 255 * x / (number-1) returns an integer value in the range from 0 to 255 inclusive.
                 * From three values we can create a color object.
                 *  Each artifact will be colored so that
                 *      The red component increases from left to right
                 *      The green component increases from bottom to top
                 *      The blue component increases from near to far
                 *
                 *  You will see a white artifact in the upper left at the back
                 *  You will see a bright red artifact at the lower right in front
                 *  You will see a bright green artifact at the upper left in front
                 *  You will see a bright blue artifact at the lower left in back
                 *  You will see a black artifact at the lower left in front
                 *
                 *  On a line from the lower left front to the upper right back you will see artifacts that
                 *  have shades of gray from black to white
                 */
                artf->surface_material.diffuse_color = cs4722::color(255 * x / (number-1),
                                                                     255 * y / (number - 1),
                                                                     255 * z / (number - 1));

                artifact_list.push_back(artf);
            }
        }
    }

    /*
     * We don't make use of the colors provided by the shape.
     */
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
    const int number_of_buffers = 1;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(b_position);

    for (auto artf : artifact_list) {
        if(!artf->shape_is_shared) {
            auto pos = artf->the_shape->positions();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
            glBufferSubData(GL_ARRAY_BUFFER,
                            4 * 4 * artf->the_shape->buffer_start,
                            4 * 4 * artf->the_shape->buffer_size,
                            pos->data());
            delete pos;
        }
    }




}



void display()
{

    static auto last_time = 0.0;
    auto time = glfwGetTime();
    auto delta_time = time - last_time;
    last_time = time;

    // uncomment the following few lines to see the frame rate computed two different ways
    static double  arate = 0.0;
    auto x = (100 * arate + 1 / delta_time) / 101;
    arate  = x;
	printf("artifact count:  %d      rate:  %.3f    arate: %.3f\n", artifact_list.size(), 1/delta_time, arate);


    for (auto artf : artifact_list) {
        artf->animate(time, delta_time);
        auto model_matrix = artf->animation_transform.matrix() * artf->world_transform.matrix();
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(model_matrix));

        /*
         * We will use the diffuse_color in the surface_material attribute of artf.
         * This is a cs4722::color object.  The as_float method produces a representation as
         *      an array of four floating point numbers in the range from 0 to 1.
         *
         *  4fv at the end of the function name indicates that
         *      4f: there are 4 float components in the uniform variable.  It's a vec4, in other words
         *      v: The values are provided as a single array or 'vector'
         */
        glUniform4fv(u_diffuse_color, 1, artf->surface_material.diffuse_color.as_float().get());

        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start, artf->the_shape->buffer_size);
       

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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Use Material", NULL, NULL);
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
