#include "model.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Model::Model(const char *filename) : verts_(), faces_(), uvs_() {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) {
		// throw std::runtime_error("file does not exist.");
		return;
	}
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		// TODO: this can be improved!
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			verts_.push_back(v);
		} else if (!line.compare(0, 2, "f ")) {
			std::vector<int> f;
			std::vector<int> fvt;
			std::vector<int> fnt;

			int ndx, idx, tdx;
			iss >> trash;
			// The 'f' leading line is just like "f 1/2/3 4/5/6 7/8/9".
			// The "1/2/3" is vertex_index/texture_index/normal_index
			// respectively. Put texture_index and normal_index into itrash.
			// Put slash into trash.
			while (iss >> idx >> trash >> tdx >> trash >> ndx) {
				idx--;
				f.push_back(idx);
				tdx--;
				fvt.push_back(tdx);
				ndx--;
				fnt.push_back(ndx);
			}
			faces_.push_back(f);
			faces_tex_.push_back(fvt);
			faces_normal_.push_back(fnt);
		} else if (!line.compare(0, 3, "vt ")) {
			iss >> trash >> trash;
			Vec2f vt;
			for (int i = 0; i < 2; i++) iss >> vt[i];
			uvs_.push_back(vt);
		} else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			Vec3f nt;
			for (int i = 0; i < 3; i++) iss >> nt[i];
			normals_.push_back(nt);
		}
	}
	std::cerr << "# v# " << verts_.size() << "# f# " << faces_.size()
			  << std::endl;
	load_texture(filename, "_diffuse.tga", diffusemap_);
	load_texture(filename, "_nm.tga", normalmap_);
	load_texture(filename, "_spec.tga", specularmap_);
}

void Model::load_texture(std::string filename, const char *suffix,
						 TGAImage &img) {
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) {
		texfile = texfile.substr(0, dot) + std::string(suffix);
		std::cerr << "texture file " << texfile << " loading "
				  << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed")
				  << std::endl;
		img.flip_vertically();
	}
}

Model::~Model() {}

/// @brief model verts num
/// @return verts num
int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

/// @brief face 3 vector
/// @param idx index of face
/// @return face
std::vector<int> Model::face(int idx) { return faces_[idx]; }

std::vector<int> Model::face_uv(int idx) { return faces_tex_[idx]; }

std::vector<int> Model::face_normal(int idx) { return faces_normal_[idx]; }

TGAColor Model::diffuse(Vec2f _uv) {
	return diffusemap_.get(int(_uv.x * diffusemap_.width()),
						   int(_uv.y * diffusemap_.height()));
}

Vec3f Model::normal(Vec2f _uv) {
	TGAColor normal_color = normalmap_.get(int(_uv.x * normalmap_.width()),
										   int(_uv.y * normalmap_.height()));
	Vec3f res;
	for (int i = 0; i < 3; i++) {
		res[i] = (float)normal_color[i] / 255.f * 2.f - 1.f;
	}
	return res;
}

float Model::specular(Vec2f _uv) {
	TGAColor color = specularmap_.get(int(_uv.x * specularmap_.width()),
									  int(_uv.y * specularmap_.height()));
	return color[0] * 1.f;
}

Vec3f Model::vert(int i) { return verts_[i]; }

Vec2f Model::uv(int i) { return uvs_[i]; }

Vec3f Model::normal(int i) { return normals_[i]; }
