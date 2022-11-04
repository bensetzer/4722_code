#include "cs4722/shape.h"


using namespace cs4722;


void shape::set_color_set(std::vector<color> new_color_set)
{
	color_set_ = std::vector<color>(new_color_set);
}

shape::~shape()
{
		
}

std::vector<cs4722::color>* shape::mixed_colors()
{
	auto num = get_size();
	auto size = color_set_.size();
	auto* colors = new std::vector<cs4722::color>;
	for(int i = 0;i < num; i++)
	{
		colors->push_back(color_set_[i % size]);
	}
	return colors;
}

std::vector<glm::vec4>* shape::tangents()
{
	return new std::vector<glm::vec4>();
}


