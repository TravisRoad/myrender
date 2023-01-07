#include "model.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Model::Model(const char *filename) : verts_(), faces_() {
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
			for (int i = 0; i < 3; i++) iss >> v.raw[i];
			verts_.push_back(v);
		} else if (!line.compare(0, 2, "f ")) {
			std::vector<int> f;
			int itrash, idx;
			iss >> trash;
			// The 'f' leading line is just like "f 1/2/3 4/5/6 7/8/9".
			// The "1/2/3" is vertex_index/texture_index/normal_index
			// perspectively. Put texture_index and normal_index into itrash.
			// Put slash into trash.
			while (iss >> idx >> trash >> itrash >> trash >> itrash) {
				idx--;
				f.push_back(idx);
			}
			faces_.push_back(f);
		}
	}
	std::cerr << "# v# " << verts_.size() << "# f# " << faces_.size()
			  << std::endl;
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

Vec3f Model::vert(int idx) { return verts_[idx]; }
