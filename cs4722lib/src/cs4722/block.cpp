
#include "cs4722/shape.h"


using namespace cs4722;


block::block()
{
    
    color_set_ = std::vector<color>({ x11::blue,x11::yellow,x11::gray,
		x11::white,x11::orange,x11::black, });
   
}


std::vector<glm::vec4>* block::positions()
{
    const float height = 1;
    const float width = 1;
    const float depth = 1;
    const auto lbf = glm::vec4(-width / 2, -height / 2, depth / 2, 1.0);
    const auto ltf = glm::vec4(-width / 2, height / 2, depth / 2, 1.0);
    const auto rtf = glm::vec4(width / 2, height / 2, depth / 2, 1.0);
    const auto rbf = glm::vec4(width / 2, -height / 2, depth / 2, 1.0);
    const auto lbb = glm::vec4(-width / 2, -height / 2, -depth / 2, 1.0);
    const auto ltb = glm::vec4(-width / 2, height / 2, -depth / 2, 1.0);
    const auto rtb = glm::vec4(width / 2, height / 2, -depth / 2, 1.0);
    const auto rbb = glm::vec4(width / 2, -height / 2, -depth / 2, 1.0);

    //  order: top, front, bottom, back, left, right
 /*   glm::vec4 vertices[] = {
        ltf, rtb, ltb, rtb, ltf, rtf,
        lbf, rtf, ltf, rtf, lbf, rbf,
        lbf, rbb, rbf, rbb, lbf, lbb,
        rtb, lbb, ltb, lbb, rtb, rbb,
        ltf, lbb, lbf, lbb, ltf, ltb,
        rbb, rtf, rbf, rtf, rbb, rtb,
    };*/

    //  order: back, top, front, bottom, left, right
    // glm::vec4 vertices[] = {
    //  rtb, lbb, ltb, lbb, rtb, rbb,
    //  ltf, rtb, ltb, rtb, ltf, rtf,
    //  lbf, rtf, ltf, rtf, lbf, rbf,
    //  lbf, rbb, rbf, rbb, lbf, lbb,
    //  ltf, lbb, lbf, lbb, ltf, ltb,
    //  rbb, rtf, rbf, rtf, rbb, rtb,
    // };

  //  order: back, top, front, bottom, left, right
    auto *vertices = new std::vector<glm::vec4>({
         rtb, lbb, ltb, lbb, rtb, rbb,
	     ltf, rtb, ltb, rtb, ltf, rtf,
	     lbf, rtf, ltf, rtf, lbf, rbf,
	     lbf, rbb, rbf, rbb, lbf, lbb,
	     ltf, lbb, lbf, lbb, ltf, ltb,
	     rbb, rtf, rbf, rtf, rbb, rtb,
        });

    return vertices;
}


std::vector<cs4722::color>* block::colors()
{
    auto n = color_set_.size();
    auto* colors = new std::vector<color>();
    for (auto i = 0; i < 6; i++) {
        for (auto j = 0; j < 6; j++) {
            colors->push_back(color_set_[i % n]);
        }
    }
    return colors;
}


std::vector<glm::vec2>* block::texture_coordinates() {
    auto t00 = glm::vec2(0,0);
    auto t01 = glm::vec2(0, texture_scale);
    auto t10 = glm::vec2(texture_scale, 0);
    auto t11 = glm::vec2(texture_scale, texture_scale);

    auto coords = new std::vector<glm::vec2>();

    for (int i = 0; i < 6; i++) {
        coords->push_back(t00);
        coords->push_back(t11);
        coords->push_back(t01);
        coords->push_back(t11);
        coords->push_back(t00);
        coords->push_back(t10);
    }
    
    return coords;
}

std::vector<glm::vec4>* block::normals()
{
    auto *rtval = new std::vector<glm::vec4>({
        //  order: back, top, front, bottom, left, right
        glm::vec4(0, 0, -1, 0),  // back
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 1, 0, 0),  // top
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 0, 1, 0),  // front
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0, 0, 1, 0),
    	glm::vec4(0, 0, 1, 0),
        glm::vec4(0, -1, 0, 0),  // bottom
        glm::vec4(0, -1, 0, 0),
        glm::vec4(0, -1, 0, 0),
        glm::vec4(0, -1, 0, 0),
        glm::vec4(0, -1, 0, 0),
        glm::vec4(0, -1, 0, 0),
        glm::vec4(-1, 0, 0, 0),  // left
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(1, 0, 0, 0),      // right
        glm::vec4(1, 0, 0, 0),
        glm::vec4(1, 0, 0, 0),
        glm::vec4(1, 0, 0, 0),
        glm::vec4(1, 0, 0, 0),
        glm::vec4(1, 0, 0, 0),
        }
        );
    return rtval;
}

std::vector<glm::vec4>* block::tangents()
{
    auto* rtval = new std::vector<glm::vec4>({
        //  order: back, top, front, bottom, left, right
        glm::vec4(0, 1, 0, 0),  // back
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, -1, 0),  // top
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 1, 0, 0),  // front
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 0, -1, 0),  // bottom
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(0, 0, -1, 0),
    	glm::vec4(0, 1, 0, 0),  // left
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),  // left
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 1, 0, 0),
        }
    );
    return rtval;

}
