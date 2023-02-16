

#define _USE_MATH_DEFINES // for C++
#include <cmath>


#include "cs4722/shape.h"
#include "cs4722/utility_glm.h"
#include "GLM/trigonometric.hpp"


using namespace cs4722;

sphere::sphere(int bands, int sides)
	: bands_(bands), sides_(sides)
{
	color_set_ = std::vector<color>({ x11::slate_blue1, x11::slate_blue4, });
}

std::vector<glm::vec4>* sphere::positions()
{

	
	const auto radius = 1.0f;
	auto* cpoints = new std::vector<glm::vec4>();

   
    //float_buffer* cpoints = new float_buffer();
    //if (this.sides < 3) {
    //    // do nothing
    //}
    //else {
    const auto tc = glm::vec4(0, radius, 0, 1);
    const auto bc = glm::vec4(0, -radius, 0, 1);
    const auto deltaU = static_cast<float>(  2 * M_PI / sides_);
    const auto b2 = this->bands_ + 2;
    const auto deltaV = static_cast<float>( M_PI / b2);
    const auto cdu = glm::cos(deltaU);
    const auto sdu = glm::sin(deltaU);
    const auto cdv = glm::cos(deltaV);
    const auto sdv = glm::sin(deltaV);



    // north pole
    auto yu = radius * cdv;
    auto rmc = radius * sdv;
    auto xul = 0.0f;
    auto zul = rmc;
    auto xur = rmc * sdu;
    auto zur = rmc * cdu;
    for (auto s = 0; s < sides_; s++) {

        cpoints->push_back(tc);
        cpoints->push_back(glm::vec4(xul, yu, zul, 1));
        cpoints->push_back(glm::vec4(xur, yu, zur, 1));

        xul = xur;
        zul = zur;
        xur = xul * cdu + zul * sdu;
        zur = zul * cdu - xul * sdu;
    }


    // sides_
    auto rmc1 = radius * sdv;
    auto yl = radius * (cdv * cdv - sdv * sdv);
    auto rmc2 = radius * 2 * sdv * cdv;
    for (auto b = 0; b < this->bands_; b++) {
        auto xul1 = 0.0f;
        zul = rmc1;
        auto xll = 0.0f;
        auto zll = rmc2;

        xur = rmc1 * sdu;
        zur = rmc1 * cdu;
        auto xlr = rmc2 * sdu;
        auto zlr = rmc2 * cdu;
        for (auto s = 0; s < this->sides_; s++) {

            cpoints->push_back(glm::vec4(xur, yu, zur, 1));
            cpoints->push_back(glm::vec4(xul1, yu, zul, 1));
            cpoints->push_back(glm::vec4(xll, yl, zll, 1));

            cpoints->push_back(glm::vec4(xur, yu, zur, 1));
            cpoints->push_back(glm::vec4(xll, yl, zll, 1));
            cpoints->push_back(glm::vec4(xlr, yl, zlr, 1));

            xul1 = xur;
            zul = zur;
            xur = xul1 * cdu + zul * sdu;
            zur = zul * cdu - xul1 * sdu;
            xll = xlr;
            zll = zlr;
            xlr = xll * cdu + zll * sdu;
            zlr = zll * cdu - xll * sdu;
        }
        yu = yl;
        yl = yu * cdv - rmc2 * sdv;
        rmc1 = rmc2;
        rmc2 = rmc1 * cdv + yu * sdv;
    }


    // south pole
    rmc = radius * sdv;
    xul = 0;
    zul = rmc;
    xur = rmc * sdu;
    zur = rmc * cdu;
    for (auto s = 0; s < this->sides_; s++) {

        cpoints->push_back(glm::vec4(xul, yu, zul, 1));
        cpoints->push_back(bc);
        cpoints->push_back(glm::vec4(xur, yu, zur, 1));

        xul = xur;
        zul = zur;
        xur = xul * cdu + zul * sdu;
        zur = zul * cdu - xul * sdu;
    }

    return cpoints;

}



std::vector<cs4722::color>* sphere::colors()
{
	auto color1 = color_set_[0];
    auto color2 = color_set_[1];
    //std::cout << "sphere getcolorbuffer" << std::endl;
    //std::cout << (std::string)this->color_set[0] << "  " << (std::string)this->color_set[1] << std::endl;
    //std::cout << (std::string)color1 << "  " << (std::string)color2 << std::endl;
   auto* colors = new std::vector<color>();
        
    // top colors
    for (int n = 0; n < this->sides_ / 2; n++) {
        colors->push_back(color1);
        colors->push_back(color1);
        colors->push_back(color1);

        colors->push_back(color2);
        colors->push_back(color2);
        colors->push_back(color2);
    }
    if (this->sides_ % 2 == 1) {
        colors->push_back(color1);
        colors->push_back(color1);
        colors->push_back(color1);
    }

    
    for (int b = 0; b < this->bands_; b++) {
            for (int s = 0; s < this->sides_; s++) {
                colors->push_back(color1);
                colors->push_back(color1);
                colors->push_back(color1);

                
                colors->push_back(color2);
                colors->push_back(color2);
                colors->push_back(color2);
            }
        }
        
    //// top colors
    //    for (int n = 0; n < this->sides / 2; n++) {
    //        colors->push_back(color1);
    //        colors->push_back(color1);
    //        colors->push_back(color1);

    //        colors->push_back(color2);
    //        colors->push_back(color2);
    //        colors->push_back(color2);
    //    }
    //    if (this->sides % 2 ==  1) {
    //        colors->push_back(color1);
    //        colors->push_back(color1);
    //        colors->push_back(color1);
    //    }
        // bottom colors
        for (int n = 0; n < this->sides_ / 2; n++) {
            colors->push_back(color1);
            colors->push_back(color1);
            colors->push_back(color1);

            colors->push_back(color2);
            colors->push_back(color2);
            colors->push_back(color2);
        }
        if (this->sides_ % 2 == 1) {
            colors->push_back(color1);
            colors->push_back(color1);
            colors->push_back(color1);
        }

        return colors;
	
}


std::vector<glm::vec2>* sphere::texture_coordinates() {
    auto coords = new std::vector<glm::vec2>();


    // s is horizontal
    // t is vertical
    //    Divide texture vertically into bands+2 parts
    float db = 1.0f / (bands_ + 2);
    // north pole uses t from 1.0-db to 1.0
    // band i (0 at the top) uses
    //      1.0 - (i+2)*db  to  1.0 - (i+1)*db
    // south pole uses t from 0.0 to db
    //
    //  south pole is band number 'bands'
    //    1 - (bands+2)*db = 1 - 1 = 0   ok
    //    1 - (bands+1)*db = 1 - (bands+1)/(bands+2) = 1 - 1 + 1/(bands+2) = db
    //
    // north pole is band number -1
    //    1 - 1*db = 1 -db
    //    1 - 0*db = 1

    float ds = 1.0f / sides_;

    // north pole
    float tFrom = 1.0f - db;
    float tTo = 1.0f;
    for (auto s = 0; s < sides_; s++) {
        float sl = 1 - s * ds;
        float sr = sl - ds;
        coords->push_back(glm::vec2((sr + sl) / 2, tTo));
        coords->push_back(glm::vec2(sl, tFrom));
        coords->push_back(glm::vec2(sr, tFrom));
    }

    // sides
    for (auto b = 0; b < bands_; b++) {
        float tFrom = 1.0f - (b + 2) * db;
        float tTo = tFrom + db;
        for (auto s = 0; s < sides_; s++) {
            auto sl = 1 - s * ds;
            auto sr = sl - ds;

            coords->push_back(glm::vec2(sr, tTo));
            coords->push_back(glm::vec2(sl, tTo));
            coords->push_back(glm::vec2(sl, tFrom));
            coords->push_back(glm::vec2(sr, tTo));
            coords->push_back(glm::vec2(sl, tFrom));
            coords->push_back(glm::vec2(sr, tFrom));
        }
    }


    // south pole
    tFrom = 0.0;
    tTo = db;
    for (auto s = 0; s < sides_; s++) {
        float sl = 1 - s * ds;
        float sr = sl - ds;
        coords->push_back(glm::vec2(sl, tTo));
        coords->push_back(glm::vec2((sl + sr) / 2, tFrom));
        coords->push_back(glm::vec2(sr, tTo));
    }


    return coords;

}


std::vector<glm::vec4>* sphere::normals()
{
    auto* verts = positions();
	for(auto i = verts->begin(); i != verts->end(); ++i)
	{
//        (*i)[3] = 0.0f;
//        i->x = -i->x;
//        i->y = -i->y;
//        i->z = -i->z;
        i->w = 0.0f;
//        i->y = -i->y;
//        i->x = 0.0f;
	}
    return verts;
}

// std::vector<glm::vec4>* sphere::normals()
// {
//     auto *cpoints = new std::vector<glm::vec4>();
//     if (sides_ < 3) {
//         // do nothing
//     }
//     else {
//         auto tc = glm::vec4(0, 1, 0, 0);
//         auto bc = glm::vec4(0, -1, 0, 0);
//         const auto deltaU = 2 * M_PI / sides_;
//         const auto deltaV = M_PI / (bands_ + 2);
//         const auto cdu = cos(deltaU);
//         const auto sdu = sin(deltaU);
//         const auto cdv = cos(deltaV);
//         const auto sdv = sin(deltaV);
//
//
//
//         // north pole
//         auto yu = cdv;
//         auto rmc = sdv;
//         auto xul = 0;
//         auto zul = rmc;
//         auto xur = rmc * sdu;
//         auto zur = rmc * cdu;
//         for (auto s = 0; s < sides_; s++) {
//             cpoints->push_back(tc);
//             cpoints->push_back(glm::vec4(xul, yu, zul, 0));
//             cpoints->push_back(glm::vec4(xur, yu, zur, 0));
//
//             xul = xur;
//             zul = zur;
//             xur = xul * cdu + zul * sdu;
//             zur = zul * cdu - xul * sdu;
//         }
//
//
//         // sides
//         auto rmc1 = sdv;
//         auto yl = (cdv * cdv - sdv * sdv);
//         auto rmc2 = 2 * sdv * cdv;
//         for (auto b = 0; b < bands_; b++) {
//             auto xul = 0;
//             auto zul = rmc1;
//             auto xll = 0;
//             auto zll = rmc2;
//
//             auto xur = rmc1 * sdu;
//             auto zur = rmc1 * cdu;
//             auto xlr = rmc2 * sdu;
//             auto zlr = rmc2 * cdu;
//             for (auto s = 0; s < sides_; s++) {
//
//                 cpoints->push_back(glm::vec4(xur, yu, zur, 0));
//                 cpoints->push_back(glm::vec4(xul, yu, zul, 0));
//                 cpoints->push_back(glm::vec4(xll, yl, zll, 0));
//
//                 cpoints->push_back(glm::vec4(xur, yu, zur, 0));
//                 cpoints->push_back(glm::vec4(xll, yl, zll, 0));
//                 cpoints->push_back(glm::vec4(xlr, yl, zlr, 0));
//
//
//                 xul = xur;
//                 zul = zur;
//                 xur = xul * cdu + zul * sdu;
//                 zur = zul * cdu - xul * sdu;
//                 xll = xlr;
//                 zll = zlr;
//                 xlr = xll * cdu + zll * sdu;
//                 zlr = zll * cdu - xll * sdu;
//             }
//             yu = yl;
//             yl = yu * cdv - rmc2 * sdv;
//             rmc1 = rmc2;
//             rmc2 = rmc1 * cdv + yu * sdv;
//         }
//
//
//         // south pole
//         rmc = sdv;
//         xul = 0;
//         zul = rmc;
//         xur = rmc * sdu;
//         zur = rmc * cdu;
//         for (auto s = 0; s < sides_; s++) {
//
//             cpoints->push_back(glm::vec4(xul, yu, zul, 0));
//             cpoints->push_back(bc);
//             cpoints->push_back(glm::vec4(xur, yu, zur, 0));
//
//             xul = xur;
//             zul = zur;
//             xur = xul * cdu + zul * sdu;
//             zur = zul * cdu - xul * sdu;
//         }
//
//         // if (this.reverse_normals) {
//         //     cpoints = cpoints.map(x = > negate(x));
//         // }
//         
//
//     }
//     return cpoints;
// }
//

