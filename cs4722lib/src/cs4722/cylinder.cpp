

#define _USE_MATH_DEFINES // for C++
#include <cmath>


#include "cs4722/shape.h"
#include "cs4722/utility_glm.h"
#include "GLM/trigonometric.hpp"


using namespace cs4722;


cylinder::cylinder(float const top_to_bottom_ratio, int const sides)
	: top_to_bottom_ratio_(top_to_bottom_ratio), sides_(sides)
{
	color_set_ = std::vector<color>({ x11::blue, x11::yellow, });
}


std::vector<glm::vec4>* cylinder::positions()
{

    auto *cpoints = new std::vector<glm::vec4>();
	
    const auto radius_bottom = 2 / (top_to_bottom_ratio_+ 1);
    const auto radius_top = this->top_to_bottom_ratio_ * radius_bottom;
    //float_buffer* cpoints = new float_buffer();

    const auto xt = 0.0f;
    const auto yt = 0.5f;
    const auto zt = 0.0f;
    const auto xb = 0.0f;
    const auto yb = -0.5f;
    const auto zb = 0.0f;

    // side vertices
    const auto delta = static_cast<float>(2 * M_PI / this->sides_);
    const auto cd = cos(delta);
    const auto sd = sin(delta);
    auto c0 = 1.0f;
    auto s0 = 0.0f;
    for (auto s = 0; s < this->sides_; s++) {
        const auto c1 = c0 * cd - s0 * sd;
        // let c1 = Math.cos((s+1)*delta);
        const  auto s1 = s0 * cd + c0 * sd;
        // let s1 = Math.sin((s+1)*delta);
        // side s, from angle s*2*pi/sides to (s+1)*2*pi/sides
        const auto x0t = c0 * radius_top;
        const auto z0t = s0 * radius_top;
        const auto x1t = c1 * radius_top;
        const auto z1t = s1 * radius_top;
        const auto x0b = c0 * radius_bottom;
        const auto z0b = s0 * radius_bottom;
        const auto x1b = c1 * radius_bottom;
        const auto z1b = s1 * radius_bottom;
        c0 = c1;
        s0 = s1;

        cpoints->push_back(glm::vec4(x1t, yt, z1t, 1));
        cpoints->push_back(glm::vec4(x0t, yt, z0t, 1));
        cpoints->push_back(glm::vec4(x1b, yb, z1b, 1));


        cpoints->push_back(glm::vec4(x0b, yb, z0b, 1));
        cpoints->push_back(glm::vec4(x1b, yb, z1b, 1));
        cpoints->push_back(glm::vec4(x0t, yt, z0t, 1));
    }

    // top
    //auto delta = 2 * M_PI / this->sides;
    for (auto s = 0; s < this->sides_; s++) {
        const auto x0 = radius_top * glm::cos(s * delta);
        const auto z0 = radius_top * glm::sin(s * delta);
        const auto x1 = radius_top * glm::cos((s + 1) * delta);
        const auto z1 = radius_top * glm::sin((s + 1) * delta);
        // cpoints.push_back([xt, yt, zt, 1]);
        cpoints->push_back(glm::vec4(x1, yt, z1, 1));
        cpoints->push_back(glm::vec4(xt, yt, zt, 1));
        cpoints->push_back(glm::vec4(x0, yt, z0, 1));
    }


    // bottom
    for (auto s = 0; s < this->sides_; s++) {
        const auto x0 = radius_bottom * glm::cos(s * delta);
        const auto z0 = radius_bottom * glm::sin(s * delta);
        const auto x1 = radius_bottom * glm::cos((s + 1) * delta);
        const auto z1 = radius_bottom * glm::sin((s + 1) * delta);
        ;
        cpoints->push_back(glm::vec4(xb, yb, zb, 1));
        cpoints->push_back(glm::vec4(x1, yb, z1, 1));
        cpoints->push_back(glm::vec4(x0, yb, z0, 1));
    }

    return cpoints;
}



std::vector<cs4722::color>* cylinder::colors()
{
	auto* colors = new std::vector<color>();

	color color1 = color_set_[0];
	color color2 = color_set_[1];
	//ubyte_buffer* colors = new ubyte_buffer();

	for (int s = 0; s < this->sides_; s++)
	{
		colors->push_back(color1);
		colors->push_back(color1);
		colors->push_back(color1);
		colors->push_back(color2);
		colors->push_back(color2);
		colors->push_back(color2);
	}

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < sides_ / 2; i++)
		{
			colors->push_back(color1);
			colors->push_back(color1);
			colors->push_back(color1);
			colors->push_back(color2);
			colors->push_back(color2);
			colors->push_back(color2);
		}
		if (sides_ % 2 == 1)
		{
			colors->push_back(color1);
			colors->push_back(color1);
			colors->push_back(color1);
		}
	}

    return colors;
}


std::vector<glm::vec2>* cylinder::texture_coordinates() {
    auto const num_sides = static_cast<float>(sides_);
    auto const divt = .95f;
    auto const divb = .05f;
    auto const zb = 0.0f;
    auto const zt = 1.0f;

    // auto const radius_top = 1.0f;
    auto cpoints = new std::vector<glm::vec2>();
    if (num_sides < 3) {
        // do nothing
    }
    else {
        auto const delta = 1.0f / num_sides;


        // sides
        for (auto s = 0; s < sides_; s++) {
            auto const x0 = s * delta;
            auto const x1 = (s + 1) * delta;

            cpoints->push_back(glm::vec2(x1, divt));
            cpoints->push_back(glm::vec2(x0, divt));
            cpoints->push_back(glm::vec2(x1, divb));

            cpoints->push_back(glm::vec2(x0, divb));
            cpoints->push_back(glm::vec2(x1, divb));
            cpoints->push_back(glm::vec2(x0, divt));
        }
    	
        // top
        for (auto s = 0; s < sides_; s++) {
            auto const x0 = s * delta;
            auto const x1 = (s + 1) * delta;
            cpoints->push_back(glm::vec2(x0, divt));
            cpoints->push_back(glm::vec2((x0 + x1) / 2, zt));
            cpoints->push_back(glm::vec2(x1, divt));
        }


    	// bottom
        for (auto s = 0; s < sides_; s++) {
            auto const x0 = s * delta;
            auto const x1 = (s + 1) * delta;
            cpoints->push_back(glm::vec2(x0, divb));
            cpoints->push_back(glm::vec2((x0 + x1) / 2, zb));
            cpoints->push_back(glm::vec2(x1, divb));
        }

    
    	
    }

    return cpoints;
}


std::vector<glm::vec4>* cylinder::normals()
{

    auto const radius_bottom = 2 / (top_to_bottom_ratio_ + 1);
    const auto radius_top = top_to_bottom_ratio_ * radius_bottom;
    auto *nrm = new std::vector<glm::vec4>();
    if (sides_ < 3) {
        // do nothing
    }
    else {
        auto yt = .5;
        auto yb = -.5;
        const auto delta = 2 * M_PI / sides_;
        const auto cd = cos(delta);
        const auto sd = sin(delta);
        auto c0 = 1.0;
        auto s0 = 0.0;
        for (auto s = 0; s < sides_; s++) {
            const auto c1 = c0 * cd - s0 * sd;
            // let c1 = Math.cos((s+1)*delta);
            const auto s1 = s0 * cd + c0 * sd;
            // let s1 = Math.sin((s+1)*delta);
            // console.log("s = " + s);
            // console.log("angle " + s * delta / Math.PI * 180);
            // side s, from angle s*2*pi/sides to (s+1)*2*pi/sides
            const auto  x0t = c0 * radius_top;
            const auto  z0t = s0 * radius_top;
            const  auto x1t = c1 * radius_top;
            const auto  z1t = s1 * radius_top;
            const  auto x0b = c0 * radius_bottom;
            const  auto z0b = s0 * radius_bottom;
            const  auto x1b = c1 * radius_bottom;
            const  auto z1b = s1 * radius_bottom;
            c0 = c1;
            s0 = s1;

            auto topFactor = -(radius_top - radius_bottom) / (1 * radius_top);
            auto bottomFactor = -(radius_top - radius_bottom) / (1 * radius_bottom);
            // cpoints.push([x1t, yt, z1t, 1]);
            // cpoints.push([x0t, yt, z0t, 1]);
            // cpoints.push([x1b, yb, z1b, 1]);
            // nrm.push(vec4(x1t, topFactor*(x1t*x1t+z1t*z1t), z1t, 0));
            // nrm.push(vec4(x0t, topFactor*(x0t*x0t+z0t*z0t), z0t, 0));
            // nrm.push(vec4(x1b, bottomFactor*(x1b*x1b+z1b*z1b), z1b, 0));
            // nrm->push_back(glm::vec4(x1t, 0, z1t, 0));
            // nrm->push_back(glm::vec4(x0t, 0, z0t, 0));
            // nrm->push_back(glm::vec4(x1b, 0, z1b, 0));
            nrm->push_back(glm::vec4(x1t, topFactor * (x1t * x1t + z1t * z1t), z1t, 0));
            nrm->push_back(glm::vec4(x0t, topFactor * (x0t * x0t + z0t * z0t), z0t, 0));
            nrm->push_back(glm::vec4(x1b, bottomFactor * (x1b * x1b + z1b * z1b), z1b, 0));

            // cpoints.push(vec4(x0b, yb, z0b, 1));
            // cpoints.push(vec4(x1b, yb, z1b, 1));
            // cpoints.push(vec4(x0t, yt, z0t, 1));
            // nrm.push(vec4(x0b, bottomFactor*(x0b*x0b+z0b*z0b), z0b, 0));
            // nrm.push(vec4(x1b, bottomFactor*(x1b*x1b+z1b*z1b), z1b, 0));
            // nrm.push(vec4(x0t, topFactor*(x0t*x0t+z0t*z0t), z0t, 0));
            // nrm->push_back(glm::vec4(x0b, 0, z0b, 0));
            // nrm->push_back(glm::vec4(x1b, 0, z1b, 0));
            // nrm->push_back(glm::vec4(x0t, 0, z0t, 0));
            nrm->push_back(glm::vec4(x0b, bottomFactor * (x0b * x0b + z0b * z0b), z0b, 0));
            nrm->push_back(glm::vec4(x1b, bottomFactor * (x1b * x1b + z1b * z1b), z1b, 0));
            nrm->push_back(glm::vec4(x0t, topFactor * (x0t * x0t + z0t * z0t), z0t, 0));
        }

        // nrm.map(v => negate(v));

        // top normals

        for (auto s = 0; s < sides_; s++) {
            // nrm.push(vec4(0, 0, 1, 0));
            // nrm.push(vec4(0, 0, 1, 0));
            // nrm.push(vec4(0, 0, 1, 0));
            nrm->push_back(glm::vec4(0, 1, 0, 0));
            nrm->push_back(glm::vec4(0, 1, 0, 0));
            nrm->push_back(glm::vec4(0, 1, 0, 0));
        }

        // bottom normals
        for (auto s = 0; s < sides_; s++) {
            // nrm.push(vec4(0, 0, -1, 0));
            // nrm.push(vec4(0, 0, -1, 0));
            // nrm.push(vec4(0, 0, -1, 0));
            nrm->push_back(glm::vec4(0, -1, 0, 0));
            nrm->push_back(glm::vec4(0, -1, 0, 0));
            nrm->push_back(glm::vec4(0, -1, 0, 0));

        }

        


    }


	
    return nrm;
}

