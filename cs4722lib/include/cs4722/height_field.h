//
// Created by Ben on 2/10/2023.
//

#pragma once

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <vector>

#include "FastNoiseLite.h"


/**
 *  \brief A height_field represents a two dimensional arrays of numbers that represent heights.
 *
 *  This base class is abstract, the `generate` function is pure and virtual.
 *  The intent is that each subclass will provide a way to generate the date in the field.
 */
class height_field {

public:
    /// The number of points in each dimension of the height field.
    int width, depth;
    /// Stores the actual height values.
    double* grid;
    /** \brief How the height field will be scaled from a size of 1 by 1 by 1.
     *
     * Used by the altitude function that returns a scaled and translated height
     * based on scaled and translated
     * coordinates.
     */
    glm::vec3 scale = glm::vec3(1,1,1);
    /** \brief How the height field will be scaled from a size of 1 by 1 by 1.
     *
     * Used by the altitude function that returns a scaled and translated height
     * based on scaled and translated
     * coordinates.
     */
    glm::vec3 translate = glm::vec3(0,0,0);

    /// Allocates space to the `grid`..
    height_field(int width, int depth)
        : width(width), depth(depth)
    {
        grid = new double[width * depth];
    }

    /// Provides data to `grid`.
    virtual void generate() = 0;

    /// Returns a scaled and translated height based on scaled and translated coordinates.
   double altitude(double x, double z);

    /// Generates a vector of triangle vertices suitable for use in shape.
    std::vector<glm::vec4>* triangle_vertices();

    /// returns the lowest point in the height field with coordinates scaled and translated.
    glm::vec4 low_point();

};


/// Implements `generate` by using the Fast Noise Light library.

/**
 * The attributes in this class are just parameters that can be set for
 * a noise generating object.
 */
class height_field_fnl : public height_field {

public:
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    FastNoiseLite::NoiseType noise_type = FastNoiseLite::NoiseType_Perlin;
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    FastNoiseLite::FractalType fractal_type = FastNoiseLite::FractalType_None;
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    double  frequency = 0.01;
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    double amp = .5;
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    int number_of_octaves = 4;
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    int gain = .5;
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    double  fractal_weighted_strength = 0.0;
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    double ping_pong_strength = 2.0;
    /// Holds a value for an attribute of FastNoiseLight noise, initialized to the default
    double fractal_lacunarity = 2.0;
    /// This is the default seed used for noise generation in Fast Noise Light
    /// Change this value to get different terrains.
    int seed = 1337;


    height_field_fnl(int width, int height)
        : height_field(width, height)
    {

    }

    virtual void generate();

private:
    /// obsolete
    void generate_v1();


};


/**
 * \brief Generates height fields using the C random number generator.
 *
 * Not all that useful
 */
class height_field_rand : public height_field {
public:

    unsigned int seed = 123456;

    height_field_rand(int width, int height)
            : height_field(width, height)
    {

    }


    virtual void generate();
};