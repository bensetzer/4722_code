//
// Created by bsetz on 2/7/2023.
//

#include "cs4722/shape.h"
//#include "cs4722/x11.h"

cs4722::terrain::terrain(height_field *heightField)
        : heightField(heightField) {
    color_set_ = std::vector<cs4722::color>({cs4722::x11::blue, cs4722::x11::green, cs4722::x11::sienna,
                                             cs4722::x11::white, cs4722::x11::white});
    divisions = std::vector<double>({0, .1, .4, .9, 1});
//    color_set_ = std::vector<cs4722::color>({cs4722::x11::blue, cs4722::x11::green, cs4722::x11::sienna });
//    divisions = std::vector<double>({0, .1, 1});
}

std::vector<glm::vec4>* cs4722::terrain::positions() {
    return heightField->triangle_vertices();
}

//
//static cs4722::color mix(double alpha, const cs4722::color & first, const cs4722::color &second) {
//    cs4722::color rtval;
//    rtval.r = (GLubyte)(alpha * first.r + (1-alpha) * second.r);
//    rtval.g = (GLubyte)(alpha * first.g + (1-alpha) * second.g);
//    rtval.b = (GLubyte)(alpha * first.b + (1-alpha) * second.b);
//    return rtval;
//}

std::vector<cs4722::color>* cs4722::terrain::colors() {
    auto v_colors = new std::vector<cs4722::color>();
    auto* position_list = positions();
    for(auto pos : *position_list) {
        int i = 1;
        while(pos.y > divisions[i]) {
            i++;
        }
        auto alpha = (pos.y-divisions[i-1])/(divisions[i]- divisions[i-1]);
//        auto col = mix(alpha, color_set_[i], color_set_[i-1]);
        auto col = color_set_[i].mix(alpha, color_set_[i-1]);
        col.a = 255;
        v_colors->push_back(col);
    }
    return v_colors;
}


std::vector<cs4722::color>* cs4722::terrain_alt_colors::colors() {
    auto v_colors = new std::vector<cs4722::color>();
    for(auto i = 0; i < get_size(); i += 6) {
        v_colors->push_back(color_set_[0]);
        v_colors->push_back(color_set_[0]);
        v_colors->push_back(color_set_[0]);
        v_colors->push_back(color_set_[1]);
        v_colors->push_back(color_set_[1]);
        v_colors->push_back(color_set_[1]);
    }
    auto* position_list = positions();
    for(auto pos : *position_list) {
        cs4722::color col;
        auto split = .1;
        if(pos.y > .1) {
            auto alpha = (pos.y-split)/(1- split);
            col.r = (int) (color_set_[0].r * alpha + color_set_[1].r * (1 - alpha));
            col.g = (int) (color_set_[0].g * alpha + color_set_[1].g * (1 - alpha));
            col.b = (int) (color_set_[0].b * alpha + color_set_[1].b * (1 - alpha));
        } else {
            auto alpha = pos.y / split;
            col.r = (int) (color_set_[1].r * alpha + color_set_[2].r * (1 - alpha));
            col.g = (int) (color_set_[1].g * alpha + color_set_[2].g * (1 - alpha));
            col.b = (int) (color_set_[1].b * alpha + color_set_[2].b * (1 - alpha));
        }
        col.a = 255;
        v_colors->push_back(col);
    }
    return v_colors;
}

