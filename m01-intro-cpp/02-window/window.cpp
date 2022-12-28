


#include <iostream>

#include <glad/gl.h>

#include <GLFW/glfw3.h>



int main(int argc, char** argv)
{
    if (!glfwInit())
        return -1;

    auto* window = glfwCreateWindow(500, 500, "Simple Window", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    int major_version, minor_version;
    glGetIntegerv(GL_MINOR_VERSION, &minor_version);
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    printf("version reported: %d.%d\n", major_version, minor_version);
    std::cout << "version reported: " << major_version << "." << minor_version << std::endl;


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}
