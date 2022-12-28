
/*
 * This example starts with the example shared shapes and animates each artifact.
 * The way rotations are set here means that you can see parts of the artifacts that are behind others.
 */

#include <algorithm>

#include "GLM/gtc/type_ptr.hpp"
#include <glad/gl.h>

#include "learnopengl/shader.h"
#include "cs4722/artifact.h"

static GLuint program;
//static GLuint transform_loc;
// locations of variables in the shaders
const static auto u_transform = 1;
const static auto b_position = 1;
const static auto b_color = 2;

static std::vector<cs4722::artifact*> artifact_list;

void init(void)
{
    Shader shader("vertex_shader-m02.glsl" ,"fragment_shader-m02.glsl" );
    program = shader.ID;
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);


/*
 * You can change which shape to use by uncommenting a differnt one of the following lines:
 */
//    auto *shape = new cs4722::sphere();
    auto *shape = new cs4722::torus();
//    auto *shape = new cs4722::block();

    auto number = 8;
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
                /*
                 * Base the rotation axis on x, y, and z so that different artifacts will have different
                 * rotation axes.
                 */
                artf->animation_transform.rotation_axis = glm::vec3(3 * x + 1, 3 * y + 1, 3 * z + 1);
                /*
                 * Rotate about the left edge of the torus.
                 */
                artf->animation_transform.rotation_center =
                        artf->world_transform.matrix() * glm::vec4(-1, 0, 0, 1);
                /*
                 * The rotation rate is larger for artifacts farther away.
                 * The pow function raises z+1 to an exponent.  This keeps the back artifacts from rotating
                 *      too quickly.
                 */
                artf->rotation_rate = M_PI / 3 * pow(z+1, .5);
                artifact_list.push_back(artf);
            }
        }
    }



    /*
     *  Set up buffers
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
    const int number_of_buffers = 2;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);


    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * total_vertex_count, NULL,  GL_STATIC_DRAW);
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
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
//    auto weight_average = 10.0;   // displayed rate is volatile reflecting the current frame
    auto weight_average = 100.0;  // displayed rate is more stable but takes longer to stabilize
    auto x = (weight_average * arate + 1 / delta_time) / (weight_average+1);
    arate  = x;
    printf("average frame rate is %f   instantaneous frame rate %f\n", arate, 1 / delta_time);


    for (auto artf : artifact_list) {
        artf->animate(time, delta_time);
        auto model_matrix = artf->animation_transform.matrix() * artf->world_transform.matrix();
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(model_matrix));
    	
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
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Animation Many", NULL, NULL);
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
