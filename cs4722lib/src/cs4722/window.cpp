//
// Created by Ben on 11/8/2020.
//


#include "GLM/common.hpp"
#include "cs4722/window.h"

namespace cs4722 {
    GLFWwindow *setup_window(const char *title, double screen_ratio, double aspect_ratio){
//        auto aspect_ratio = 1600.0f/900.0f;
        float ratio = (float) screen_ratio;
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        int xpos, ypos, width, height;
        glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
        //std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
        int w_width = glm::min(ratio * width, (float)aspect_ratio * ratio * height);
        int w_height = w_width / aspect_ratio;
        GLFWwindow* window = glfwCreateWindow(w_width, w_height, "User Interaction", NULL, NULL);
        int w_x = (width - w_width) / 2;
        int w_y = (height - w_height) / 2;
        glfwSetWindowPos(window, w_x, w_y);
        glfwMakeContextCurrent(window);
        return window;


//        auto *primary = glfwGetPrimaryMonitor();
//        int xpos, ypos, width, height;
//        glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
//        //std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
//        auto const size = glm::min(width, height);
////    auto const  ratio = .5f;
//        auto const w_height = static_cast<int>(screen_ratio * size);
//        auto const w_width = w_height * aspect_ratio;
//        auto const w_y = static_cast<int>(size * (1 - screen_ratio) / 2);
//        auto const w_x = w_y;
//
//
//        auto *window = glfwCreateWindow(w_width, w_height, title,
//                                        nullptr, nullptr);
//        glfwSetWindowPos(window, w_x, w_y);
//
//        glfwMakeContextCurrent(window);
//        return window;
    }
}