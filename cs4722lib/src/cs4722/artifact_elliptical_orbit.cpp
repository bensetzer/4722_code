//
// Created by bsetz on 4/7/2021.
//

#include "cs4722/artifact_elliptical_orbit.h"

cs4722::artifact_elliptical_orbit::artifact_elliptical_orbit()
{}

void cs4722::artifact_elliptical_orbit::animate(double time, double delta_time){
    artifact_rotating::animate(time, delta_time);
    float c = cos(orbital_rate * (time + time_offset));
    float s = sin(orbital_rate * (time + time_offset));
    animation_transform.translate = path_center +  c * perihelion_vector + s * aphelion_vector;
}