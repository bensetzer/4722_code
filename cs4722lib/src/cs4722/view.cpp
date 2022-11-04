#include "cs4722/view.h"


#include <iostream>


#include "cs4722/utility_glm.h"
#include "GLM/ext/matrix_transform.hpp"

using namespace cs4722;


//static bool enable_logging = false;

void view::display_parameters() const
{
    std::cout << enable_logging << "    f l u p " << camera_forward << ", "
        << camera_left << ",   " << camera_up << ",  " << camera_position
        << std::endl;
}


void view::set_camera_position(glm::vec3 const position) {
    camera_position = position;
    if (enable_logging) {
        std::cout << "set camera position" << std::endl;
        display_parameters();
    }

}

void view::reverse_camera()
{
    camera_forward = -camera_forward;
    camera_left = -camera_left;
}


void view::set_flup(glm::vec3 forward, glm::vec3 left, glm::vec3 up,
    glm::vec3 position)
{
    auto EPS = 1e-5;
    if (glm::dot(forward, left) > EPS
        || glm::dot(forward, up) > EPS
        || glm::dot(up, left) > EPS)
    {
        // throw "set_flup vectors not orthogonal to each other";
        std::cout << "set_flup vectors not orthogonal to each other" << std::endl;
        std::cout << glm::dot(forward, left) << std::endl;
        std::cout << glm::dot(forward, up) << std::endl;
        std::cout << glm::dot(up, left) << std::endl;
    }
    if (glm::abs(glm::length(forward) - 1.0f) > EPS
		|| glm::abs(glm::length(up) - 1.0f) > EPS
        || glm::abs(glm::length(left) - 1.0f) > EPS
        )
    {
        // throw "set_flup vectors not unit length";
        std::cout << "set_flup vectors not unit length" << std::endl;
        std::cout << (glm::length(forward)-1) << std::endl;
        std::cout << (glm::length(up) - 1) << std::endl;
        std::cout << (glm::length(left) - 1) << std::endl;
    }
    camera_up = up;
    camera_left = left;
    camera_position = position;
    camera_forward = forward;
}