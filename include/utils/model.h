#ifndef __MODEL_H__
#define __MODEL_H__

#include "tgaimage.h"
#include "vectors.h"
#include <vector>

class Model {
  private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> uvs_;
	std::vector<Vec3f> normals_; // vertex normal
	std::vector<std::vector<int>> faces_;
	std::vector<std::vector<int>> faces_tex_;
	std::vector<std::vector<int>> faces_normal_;
	TGAImage diffusemap_;
	TGAImage normalmap_;
	TGAImage specularmap_;
	void load_texture(std::string filename, const char *suffix, TGAImage &img);

  public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec3f uv(int i);
	Vec3f normal(int i);
	std::vector<int> face(int idx);
	std::vector<int> face_uv(int idx);
	std::vector<int> face_normal(int idx);
};

#endif
