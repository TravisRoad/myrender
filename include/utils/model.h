#ifndef __MODEL_H__
#define __MODEL_H__

#include "vectors.h"
#include <vector>

class Model {
  private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> vts_;
	std::vector<Vec3f> vns_; // vertex normal
	std::vector<std::vector<int>> faces_;
	std::vector<std::vector<int>> faces_tex_;
	std::vector<std::vector<int>> faces_normal_;

  public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	int nvts();
	int nvns();
	Vec3f vert(int i);
	Vec3f vts(int i);
	Vec3f vns(int i);
	std::vector<int> face(int idx);
	std::vector<int> face_tex(int idx);
	std::vector<int> face_normals(int idx);
};

#endif
