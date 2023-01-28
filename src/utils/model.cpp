#include "model.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Model::Model(const char *filename) : verts_(), faces_(), vts_() {
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
			Vec3f vt;
			for (int i = 0; i < 3; i++) iss >> vt[i];
			vts_.push_back(vt);
		} else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			Vec3f nt;
			for (int i = 0; i < 3; i++) iss >> nt[i];
			vns_.push_back(nt);
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

int Model::nvts() { return (int)vts_.size(); }

int Model::nvns() { return (int)vns_.size(); }

/// @brief face 3 vector
/// @param idx index of face
/// @return face
std::vector<int> Model::face(int idx) { return faces_[idx]; }

std::vector<int> Model::face_tex(int idx) { return faces_tex_[idx]; }

std::vector<int> Model::face_normals(int idx) { return faces_normal_[idx]; }

Vec3f Model::vert(int i) { return verts_[i]; }

Vec3f Model::vts(int i) { return vts_[i]; }

Vec3f Model::vns(int i) { return vns_[i]; }
