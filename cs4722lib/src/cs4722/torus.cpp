#include "cs4722/shape.h"
#include "GLM/glm.hpp"
#include "cs4722/utility_glm.h"


using namespace cs4722;

torus_points::torus_points(float inner_radius, int bands, int sides) 
	: inner_radius_(inner_radius), bands_(bands), sides_(sides)
{
	drawing_mode = GL_POINTS;
}

int torus_points::get_size() {
	return bands_ * sides_;
}

/*
Get vertices
*/
std::vector<glm::vec4>* torus_points::positions() {
	auto vertices = new std::vector<glm::vec4>();
	float R = (inner_radius_ + 1) / 2;
	float r = (1 - inner_radius_) / 2;
	for (int iv = 0; iv < bands_; iv++) {
		for (int iu = 0; iu < sides_; iu++) {
			auto v = iv * 2 * M_PI / bands_;
			auto u = iu * 2 * M_PI / sides_;
			float x = glm::cos(u) * (r * glm::cos(v) + R);
			float y = glm::sin(u) * (r * glm::cos(v) + R);
			float z = r * sin(v);
			vertices->push_back(glm::vec4(x, y, z, 1.0f));
		}
	}
	return vertices;
}



std::vector<cs4722::color>* torus_points::colors() {
	auto *colors = new std::vector<cs4722::color>();
	float R = (inner_radius_ + 1) / 2;
	float r = (1 - inner_radius_) / 2;
	for (int iv = 0; iv < bands_; iv++) {
		for (int iu = 0; iu < sides_; iu++) {
			auto v = iv * 2 * M_PI / bands_;
			auto u = iu * 2 * M_PI / sides_;
			auto c1 = cs4722::color(0, 255*cos(v), 255*cos(u), 255);
			colors->push_back(c1);
		}
	}

	return colors;
}



//
//    Class torus
//

torus::torus(float inner_radius, int bands, int sides)
	: inner_radius_(inner_radius), bands_(bands), sides_(sides)
{
	color_set_ = std::vector<color>({ x11::olive_drab3, x11::olive_drab4, });
}

int torus::get_size() {
	return 6 * bands_ * sides_;
}

/*
Get vertices
*/
std::vector<glm::vec4>* torus::positions() {
	auto *vertices = new std::vector<glm::vec4>();
	float R = (inner_radius_ + 1) / 2;
	float r = (1 - inner_radius_) / 2;
	for (int iv = 0; iv < bands_; iv++) {
		for (int iu = 0; iu < sides_; iu++) {
			auto v00 = iv * 2 * M_PI / bands_;
			auto u00 = iu * 2 * M_PI / sides_;
			auto v01 = v00 + 2 * M_PI / bands_;
			auto u01 = u00;
			auto v10 = v00;
			auto u10 = u00 + 2 * M_PI / sides_;
			auto v11 = v01;
			auto u11 = u10;

			float x00 = cos(u00) * (r * cos(v00) + R);
			float y00 = sin(u00) * (r * cos(v00) + R);
			float z00 = r * sin(v00);

			float x01 = cos(u01) * (r * cos(v01) + R);
			float y01 = sin(u01) * (r * cos(v01) + R);
			float z01 = r * sin(v01);

			float x10 = cos(u10) * (r * cos(v10) + R);
			float y10 = sin(u10) * (r * cos(v10) + R);
			float z10 = r * sin(v10);

			float x11 = cos(u11) * (r * cos(v11) + R);
			float y11 = sin(u11) * (r * cos(v11) + R);
			float z11 = r * sin(v11);

			auto ll = glm::vec4(x00, y00, z00, 1.0f);
			auto ul = glm::vec4(x01, y01, z01, 1.0f);
			auto lr = glm::vec4(x10, y10, z10, 1.0f);
			auto ur = glm::vec4(x11, y11, z11, 1.0f);

			vertices->push_back(ll);
			vertices->push_back(ur);
			vertices->push_back(ul);

			vertices->push_back(ur);
			vertices->push_back(ll);
			vertices->push_back(lr);
		}
	}
	return vertices;
}



//std::vector<cs4722::color*>* torus::get_colors() {
//	auto colors = new std::vector<cs4722::color*>();
//
//	float R = (inner_radius_ + 1) / 2;
//	float r = (1 - inner_radius_) / 2;
//	for (int iv = 0; iv < bands_; iv++) {
//		for (int iu = 0; iu < sides_; iu++) {
//			auto v00 = iv * 2 * M_PI / bands_;
//			auto u00 = iu * 2 * M_PI / sides_;
//			auto v01 = v00 + 2 * M_PI / bands_;
//			auto u01 = u00;
//			auto v10 = v00;
//			auto u10 = u00 + 2 * M_PI / sides_;
//			auto v11 = v01;
//			auto u11 = u10;
//
//		
//			auto ll = new cs4722::color(0, 255 * cos(u00), 255 * cos(v00), 255);
//			auto ul = new cs4722::color(0, 255 * cos(u01), 255 * cos(v01), 255);
//			auto lr = new cs4722::color(0, 255 * cos(u10), 255 * cos(v10), 255);
//			auto ur = new cs4722::color(0, 255 * cos(u11), 255 * cos(v11), 255);
//
//			colors->push_back(ll);
//			colors->push_back(ur);
//			colors->push_back(ul);
//
//			colors->push_back(ur);
//			colors->push_back(ll);
//			colors->push_back(lr);
//		}
//	}
//
//
//	return colors;
//}


std::vector<cs4722::color>* torus::colors() {
	auto colors = new std::vector<cs4722::color>();

	float R = (inner_radius_ + 1) / 2;
	float r = (1 - inner_radius_) / 2;
	for (int iv = 0; iv < bands_; iv++) {
		for (int iu = 0; iu < sides_; iu++) {
			
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


std::vector<glm::vec2>* torus::texture_coordinates()
{
	auto* texture_map = new std::vector<glm::vec2>();
	// auto const R = (inner_radius_ + 1) / 2;
	// auto const r = (1 - inner_radius_) / 2;
	for (int iv = 0; iv < bands_; iv++) {
		for (int iu = 0; iu < sides_; iu++) {
			auto v00 = static_cast<float>(iv) / static_cast<float>(bands_);
			auto u00 = static_cast<float>(iu) / static_cast<float>(sides_);
			auto v01 = static_cast<float>(iv + 1)/ static_cast<float>(bands_);
			auto u01 = u00;
			auto v10 = v00;
			auto u10 = static_cast<float>(iu + 1) / static_cast<float>(sides_);
			auto v11 = v01;
			auto u11 = u10;


			auto ll = glm::vec2(u00, v00);
			auto ul = glm::vec2(u01, v01);
			auto lr = glm::vec2(u10, v10);
			auto ur = glm::vec2(u11, v11);

			texture_map->push_back(ll);
			texture_map->push_back(ur);
			texture_map->push_back(ul);

			texture_map->push_back(ur);
			texture_map->push_back(ll);
			texture_map->push_back(lr);
		}
	}
	return texture_map;
}


std::vector<glm::vec4>* torus::normals()
{
	auto* normals = new std::vector<glm::vec4>();
	float R = (inner_radius_ + 1) / 2;
	float r = (1 - inner_radius_) / 2;
	for (int iv = 0; iv < bands_; iv++) {
		for (int iu = 0; iu < sides_; iu++) {
			auto v00 = iv * 2 * M_PI / bands_;
			auto u00 = iu * 2 * M_PI / sides_;
			auto v01 = v00 + 2 * M_PI / bands_;
			auto u01 = u00;
			auto v10 = v00;
			auto u10 = u00 + 2 * M_PI / sides_;
			auto v11 = v01;
			auto u11 = u10;

			float x00 = cos(u00) * (cos(v00));
			float y00 = sin(u00) * (cos(v00));
			float z00 = sin(v00);

			float x01 = cos(u01) * (cos(v01));
			float y01 = sin(u01) * (cos(v01));
			float z01 = sin(v01);

			float x10 = cos(u10) * (cos(v10));
			float y10 = sin(u10) * (cos(v10));
			float z10 = sin(v10);

			float x11 = cos(u11) * (cos(v11));
			float y11 = sin(u11) * (cos(v11));
			float z11 = sin(v11);

			auto ll = glm::vec4(x00, y00, z00, 0.0f);
			auto ul = glm::vec4(x01, y01, z01, 0.0f);
			auto lr = glm::vec4(x10, y10, z10, 0.0f);
			auto ur = glm::vec4(x11, y11, z11, 0.0f);

			normals->push_back(ll);
			normals->push_back(ur);
			normals->push_back(ul);

			normals->push_back(ur);
			normals->push_back(ll);
			normals->push_back(lr);
		}
	}
	return normals;
}


std::vector<glm::vec4>* torus::tangents()
{
	auto* tangents = new std::vector<glm::vec4>();
	float R = (inner_radius_ + 1) / 2;
	float r = (1 - inner_radius_) / 2;
	for (int iv = 0; iv < bands_; iv++) {
		for (int iu = 0; iu < sides_; iu++) {
			auto v00 = iv * 2 * M_PI / bands_;
			auto u00 = iu * 2 * M_PI / sides_;
			auto v01 = v00 + 2 * M_PI / bands_;
			auto u01 = u00;
			auto v10 = v00;
			auto u10 = u00 + 2 * M_PI / sides_;
			auto v11 = v01;
			auto u11 = u10;

			float x00 = cos(u00) * (cos(v00));
			float y00 = sin(u00) * (cos(v00));
			float z00 = sin(v00);

			float x01 = cos(u01) * (cos(v01));
			float y01 = sin(u01) * (cos(v01));
			float z01 = sin(v01);

			float x10 = cos(u10) * (cos(v10));
			float y10 = sin(u10) * (cos(v10));
			float z10 = sin(v10);

			float x11 = cos(u11) * (cos(v11));
			float y11 = sin(u11) * (cos(v11));
			float z11 = sin(v11);

			auto ll = glm::vec4(-y00, x00, 0.0f, 0.0f);
			auto ul = glm::vec4(-y01, x01, 0.0f, 0.0f);
			auto lr = glm::vec4(-y10, x10, 0.0f, 0.0f);
			auto ur = glm::vec4(-y11, x11, 0.0f, 0.0f);

			tangents->push_back(ll);
			tangents->push_back(ur);
			tangents->push_back(ul);

			tangents->push_back(ur);
			tangents->push_back(ll);
			tangents->push_back(lr);
		}
	}
	return tangents;
}



std::vector<glm::vec4>* torus_points::normals()
{
	return nullptr;
}


