#include "cs4722/shape.h"

using namespace cs4722;

int rectangle::get_size() {
	return 6 * width * height;
}


std::vector<glm::vec4>* rectangle::positions() {
	auto *vertex_list = new std::vector<glm::vec4>();

	auto dx = glm::vec4(1.0f / static_cast<float>(width), 0.0f, 0.0f, 0.0f);
	auto dy = glm::vec4(0.0f, 1.0f / static_cast<float>(height), 0.0f, 0.0f);
	for (int ix = 0; ix < width; ix++) {
		for (int iy = 0; iy < height; iy++) {
			auto ll = dx * static_cast<float>(ix) + dy * static_cast<float>(iy)
				+ glm::vec4(0,0,0,1);
			auto ul = ll + dy;
			auto lr = ll + dx;
			auto ur = lr + dy;
			vertex_list->push_back(ll);
			vertex_list->push_back(ur);
			vertex_list->push_back(ul);
			vertex_list->push_back(ur);
			vertex_list->push_back(ll);
			vertex_list->push_back(lr);
		}
	}
	return vertex_list;
}

std::vector<cs4722::color>* rectangle::colors() {
	auto* colors = new std::vector<cs4722::color>();
	for (int ix = 0; ix < width; ix++) {
		for (int iy = 0; iy < height; iy++) {
			colors->push_back(color_set_[0]);
			colors->push_back(color_set_[0]);
			colors->push_back(color_set_[0]);
			colors->push_back(color_set_[1]);
			colors->push_back(color_set_[1]);
			colors->push_back(color_set_[1]);
		}
	}
	return colors;
}

std::vector<glm::vec2>* rectangle::texture_coordinates() {
	auto* texture_coords = new std::vector<glm::vec2>();

	auto dx = glm::vec2(texture_width_scale / width, 0.0f);
	auto dy = glm::vec2(0.0f, texture_height_scale / height);
	for (int ix = 0; ix < width; ix++) {
		for (int iy = 0; iy < height; iy++) {
			auto ll = dx * static_cast<float>(ix) + dy * static_cast<float>(iy);
			auto ul = ll + dy;
			auto lr = ll + dx;
			auto ur = lr + dy;
			texture_coords->push_back(ll);
			texture_coords->push_back(ur);
			texture_coords->push_back(ul);
			texture_coords->push_back(ur);
			texture_coords->push_back(ll);
			texture_coords->push_back(lr);
		}
	}
	return texture_coords;
}


std::vector<glm::vec4>* rectangle::normals()
{
	auto* normal_list = new std::vector<glm::vec4>();
	for (int i = 0; i < get_size(); ++i)
	{
		normal_list->push_back(glm::vec4(0, 0, 1, 0));
	}
	return normal_list;
}

