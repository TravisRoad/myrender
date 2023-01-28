#include "model.h"
#include "tgaimage.h"
#include "triangle.h"
#include <iostream>
#include <limits>
#include <vector>

#define NUM 1

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const int width = 800;
const int height = 800;
const int depth = 255;
const Vec3f light_dir(0, 0, -1);
const Vec3f camera(1, 1, 3);
const Vec3f center(0, 0, 0);

// x, y for translate, w, h for scale
Matrix viewport(float x, float y, float w, float h) {
	Matrix ret = Matrix::identity();
	ret[0][0] = w / 2.f;
	ret[1][1] = h / 2.f;
	ret[2][2] = depth / 2.f;

	ret[0][3] = x + w / 2.f;
	ret[1][3] = y + h / 2.f;
	ret[2][3] = depth / 2.f;
	return ret;
}

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
	Vec3f z = (eye - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();
	Matrix Minv = Matrix::identity();
	Matrix Translate = Matrix::identity();
	for (int i = 0; i < 3; i++) {
		Minv[0][i] = x[i];
		Minv[1][i] = y[i];
		Minv[2][i] = z[i];
		Translate[i][3] = -center[i];
	}
	return Minv * Translate;
}

Vec4f v3to4f(Vec3f v3) {
	Vec4f ret;
	ret[0] = v3[0];
	ret[1] = v3[1];
	ret[2] = v3[2];
	ret[3] = 1;
	return ret;
}

Vec3f v4to3f(Vec4f v4) {
	Vec3f ret;
	if (v4[3] == 1.f) {
		ret[0] = v4[0];
		ret[1] = v4[1];
		ret[2] = v4[2];
	} else {
		ret[0] = v4[0] / v4[3];
		ret[1] = v4[1] / v4[3];
		ret[2] = v4[2] / v4[3];
	}
	return ret;
}

void drawModel(Model *model, TGAImage &image, TGAImage &tex, float *zbuffer) {
	Matrix projection = Matrix::identity();
	projection[3][2] = -1.0f / (camera - center).norm();
	Matrix Viewport =
		viewport(width / 8.f, height / 8.f, width * 3 / 4.f, height * 3 / 4.f);
	Matrix ModelView = lookat(camera, center, Vec3f(0, 1, 0));

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		std::vector<int> face_tex = model->face_tex(i);
		Vec3f screen_coords[3];
		Vec3f world_coords[3];
		Vec3f tex_uvs[3];
		for (int j = 0; j < 3; j++) {
			Vec3f v = model->vert(face[j]);
			screen_coords[j] =
				v4to3f(Viewport * projection * ModelView * v3to4f(v));
			world_coords[j] = v;

			// texture
			Vec3f vt = model->vts(face_tex[j]);
			tex_uvs[j] = vt;
		}
		Vec3f normal = cross((world_coords[2] - world_coords[0]),
							 (world_coords[1] - world_coords[0]));
		normal.normalize();
		float intensity = normal * light_dir;
		if (intensity > 0) {
			triangle(screen_coords, tex_uvs, zbuffer, image, tex, intensity);
		}
	}
}

int main(int argc, char **argv) {
	TGAImage image(width, height, TGAImage::RGB);
	TGAImage texture;
	Model model("../assets/obj/african_head.obj");

	texture.read_tga_file("../assets/tex/african_head_diffuse.tga");
	texture.flip_vertically();

	float *zbuffer = new float[width * height];
	for (int i = width * height - 1; i >= 0; i--)
		zbuffer[i] = -std::numeric_limits<float>::max(); // -INF

	drawModel(&model, image, texture, zbuffer);

	{
		TGAImage zbImage(width, height, TGAImage::RGB);
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				int zval = int(zbuffer[x + y * image.width()]);
				zbImage.set(x, y, TGAColor(zval, zval, zval, 255));
			}
		}
		zbImage.write_tga_file("zbuffer.tga");
	}

	// image.flip_vertically();
	image.write_tga_file("output.tga");

	delete zbuffer;
	return 0;
}
