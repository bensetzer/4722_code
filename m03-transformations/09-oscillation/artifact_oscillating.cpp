//
// Created by Ben on 3/19/2021.
//

#include "artifact_oscillating.h"

void artifact_oscillating::animate(double time, double delta_time) {
    /*
     * This formula yields a value between 0 and 1 inclusive.
     */
    double x = (1 + sin(time * 2 * M_PI * rate)) * .5;
    /*
     * The mix function takes a weighted average of end0 and end1:
     *    (1-x) * end0 + X * end1
     *
     * As x varies from 0 to 1 then the resulting value varies from end0 to end1.
     */
    animation_transform.translate = glm::mix(end0, end1, x);
}