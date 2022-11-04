//
// Created by Ben on 11/6/2020.
//

#include "cs4722/transform.h"

using namespace cs4722;

glm::mat4 transform::matrix() {
    glm::mat4 other = glm::mat4(1.0f); // identity
    other = glm::translate(other, translate);
    other = glm::translate(other, rotation_center*scale);
    other = glm::rotate(other, rotation_angle, rotation_axis);
    other = glm::translate(other, -rotation_center*scale);
    other = glm::scale(other, scale);
    return other;
}