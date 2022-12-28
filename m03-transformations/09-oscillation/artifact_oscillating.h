//
// Created by Ben on 3/19/2021.
//

/*
 * This class animates an artifact by moving it back and forth between two specified endpoints at a specified
 * rate.
 * The movement uses the sine function to compute position based on time.
 * In this way the movement simulates a body connected to a simple spring or a body being moved by a
 * rotating wheel.
 * Using the sine is also an easy way to get an oscillating value from the steadily increasing value of time.
 *
 */

#pragma once

#include "cs4722/artifact.h"

class artifact_oscillating : public cs4722::artifact {
public:

    /*
     * The two ends of the path taken by the artifact.
     */
    glm::vec4 end0, end1;
    /*
     * The rate at which the artifact moves.
     * The artifact will complete a cycle from one endpoint to the other and back again
     * in 1/rate seconds.
     */
    double rate;


    void animate(double time, double delta_time ) override;
};

