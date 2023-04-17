//
// Created by Ben on 2/10/2023.
//

#include <cstdio>
#include <cstdlib>
#include "cs4722/height_field.h"


glm::vec4 height_field::low_point(){
    auto min_index = 0;
    for(auto k = 0; k < width*depth; k++) {
        if(grid[k] < grid[min_index]) {
            min_index = k;
        }
    }
    auto i = min_index / width;
    auto j = min_index % width;
    glm::vec4 rtval;
    rtval.x = i * scale.x / (width-1) + translate.x;
    rtval.z = j * scale.z / (depth-1) + translate.z;
    rtval.y = grid[min_index] * scale.y + translate.y;
    rtval.w = 1;
    return rtval;
}

double height_field::altitude(double x, double z) {
    auto deltax = scale.x / (width-1);
    auto deltaz = scale.z / (depth-1);
    auto i = (int)((x - translate.x) / deltax);
    auto j = (int)((z - translate.z) / deltaz);
    auto x_norm = (x - translate.x) / deltax - i;
    auto z_norm = (z - translate.z) / deltaz - j;
    auto hgt = 0.0;
    if(x_norm + z_norm <= 1) {
        hgt = x_norm * grid[(i+1)*width+j] + z_norm * grid[i*width+j+1] + (1-x_norm-z_norm) * grid[i*width+j];
    } else {
        hgt = (1 - z_norm) * grid[(i+1)*width+j] + (1 - x_norm) * grid[i*width+j+1]
                + (x_norm + z_norm - 1) * grid[(i+1)*width+j+1];
    }
    return hgt * scale.y + translate.y;
}

void height_field_fnl::generate() {
    FastNoiseLite noise(seed);
    noise.SetNoiseType(noise_type);
    noise.SetFractalType(fractal_type);
    noise.SetFrequency(frequency);
    noise.SetFractalOctaves(number_of_octaves);
    noise.SetFractalGain(gain);
    noise.SetFractalWeightedStrength(fractal_weighted_strength);
    noise.SetFractalLacunarity(fractal_lacunarity);
    noise.SetFractalPingPongStrength(ping_pong_strength);

    auto inc0 = 1.0f / (width / frequency);
    auto inc1 = 1.0f / (depth / frequency);

    for (auto i = 0; i < width; ++i)
    {
        for (auto j = 0; j < depth; ++j)
        {
            grid[i*width+j] = noise.GetNoise(i * inc0, j * inc1);
        }
    }

    //    printf("grid before normalizing\n");
//    for(auto i = 0; i < limit; i++ ) {
//        for(auto j = 0; j < limit; j++ ) {
//            printf("%f ", grid[i*limit + j]);
//        }
//        printf("\n");
//    }


    auto max_z = -100.0;
    auto min_z = 100.0;
    for(auto i = 0; i < width * depth; i++) {
        max_z = std::max(max_z, grid[i]);
        min_z = std::min(min_z, grid[i]);
    }

//    printf("min  %f   max %f\n\n", min_z, max_z);


    auto range = max_z - min_z;
    for(auto i = 0; i < width * depth; i++) {
        grid[i] = (grid[i] - min_z) / range;
    }
//    printf("grid after normalizing\n");
//    for(auto i = 0; i < width; i++ ) {
//        for(auto j = 0; j < height; j++ ) {
//            printf("%f ", grid[i*width + j]);
//        }
//        printf("\n");
//    }
//    printf("\n");


}

void height_field_fnl::generate_v1() {
    for(auto i = 0; i < depth * width; i++ ) {
        grid[i] = 0.0;
    }

//    auto  frequency = 32.0;
//    auto amp = .5;
//    auto const number_of_octaves = 4;

    FastNoiseLite noise(seed);
//    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetNoiseType(noise_type);
//    noise.SetSeed(seed);
    noise.SetFrequency(frequency);
    for (auto f = 0; f < number_of_octaves; ++f)
    {
        // auto ni = glm::vec3(0.0);
        auto inc0 = 1.0f / (width / frequency);
        auto inc1 = 1.0f / (depth / frequency);

        auto* ptr = grid;

        for (auto i = 0; i < width; ++i)
        {
            for (auto j = 0; j < depth; ++j)
            {
                auto sample = (1 +  noise.GetNoise(i * inc0, j * inc1)) * amp / 2;
                grid[i*width+j] += sample;
            }
        }
        frequency *= 2;
//        ++inc0;
//        ++inc1;
        amp *= 0.5;
    }

//    printf("grid before normalizing\n");
//    for(auto i = 0; i < limit; i++ ) {
//        for(auto j = 0; j < limit; j++ ) {
//            printf("%f ", grid[i*limit + j]);
//        }
//        printf("\n");
//    }


    auto max_z = -100.0;
    auto min_z = 100.0;
    for(auto i = 0; i < width * depth; i++) {
        max_z = std::max(max_z, grid[i]);
        min_z = std::min(min_z, grid[i]);
    }

//    printf("min  %f   max %f\n\n", min_z, max_z);


    auto range = max_z - min_z;
    for(auto i = 0; i < width * depth; i++) {
        grid[i] = (grid[i] - min_z) / range;
    }
//    printf("grid after normalizing\n");
//    for(auto i = 0; i < width; i++ ) {
//        for(auto j = 0; j < height; j++ ) {
//            printf("%f ", grid[i*width + j]);
//        }
//        printf("\n");
//    }
//    printf("\n");


}


void height_field_rand::generate() {
    for(auto i = 0; i < depth * width; i++ ) {
        grid[i] = 0.0;
    }
    srand(seed);
    double scale = 1.0 / RAND_MAX;

    for (auto i = 0; i < width; ++i)
    {
        for (auto j = 0; j < depth; ++j)
        {
            grid[i*width + j] = rand() * scale;
        }
    }

//    printf("grid before normalizing\n");
//    for(auto i = 0; i < width; i++ ) {
//        for(auto j = 0; j < height; j++ ) {
//            printf("%f ", grid[i*width + j]);
//        }
//        printf("\n");
//    }



//    printf("min  %f   max %f\n\n", min_z, max_z);


//    printf("grid after normalizing\n");
//    for(auto i = 0; i < width; i++ ) {
//        for(auto j = 0; j < height; j++ ) {
//            printf("%f ", grid[i*width + j]);
//        }
//        printf("\n");
//    }
//    printf("\n");


}







std::vector<glm::vec4>* height_field::triangle_vertices() {
    auto w = (double)(width-1);
    auto h = (double)(depth - 1);
    auto positions = new std::vector<glm::vec4>();
    for(auto i = 0; i < width-1; i++) {
        for(auto j = 0; j < depth - 1; j++) {
            auto ll = glm::vec4((i+1)/w,  grid[(i+1)*width+j], j/h,  1);
            auto ul = glm::vec4(i/w,  grid[i*width+j], j/h, 1);
            auto lr = glm::vec4((i+1)/w,  grid[(i+1)*width+j+1], (j+1)/h, 1);
            auto ur = glm::vec4(i/w,  grid[i*width+j+1], (j+1)/h, 1);
            // t1   ul lr ur
            // t2   lr ul ll
            positions->push_back(ul);
            positions->push_back(ur);
            positions->push_back(lr);

            positions->push_back(lr);
            positions->push_back(ll);
            positions->push_back(ul);
        }
    }
    return positions;
}

