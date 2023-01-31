#include "model.h"
#include "my_gl.h"
#include "tgaimage.h"
#include <iostream>
#include <limits>
#include <vector>

const int width = 800;
const int height = 800;

const Vec3f light_dir = Vec3f(1, -1, 1).normalize();
const Vec3f camera(1, 1, 3);
const Vec3f center(0, 0, 0);
const Vec3f up(0, 1, 0);

Model *model = NULL;

struct GouraudShader : public Shader {
	Vec3f varying_intensity;

	virtual Vec4f vertex(int iface, int nthvert) {
		int vt_idx = model->face(iface)[nthvert];
		int normal_idx = model->face_normal(iface)[nthvert];
		Vec4f Vertex = embed<4>(model->vert(vt_idx));
		Vertex = Viewport * Projection * ModelView * Vertex;
		varying_intensity[nthvert] =
			std::max(0.f, model->normal(normal_idx) * light_dir);
		return Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor &color) {
		float intensity = varying_intensity * bar;
		// intensity = 0.5f;
		color = TGAColor(255, 255, 255, 255) * intensity;
		return false;
	}
};

int main(int argc, char const *argv[]) {
	model = new Model("../assets/obj/african_head.obj");

	TGAImage zbufferImage(width, height, TGAImage::GRAYSCALE);
	TGAImage outputImage(width, height, TGAImage::RGB);

	lookat(camera, center, up);
	viewport(width / 8.f, height / 8.f, width * 3.f / 4.f, height * 3.f / 4.f);
	projection(-1.f / (camera - center).norm());

	GouraudShader shader;
	for (int i = 0; i < model->nfaces(); i++) {
		Vec4f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, outputImage, zbufferImage);
		// outputImage.write_tga_file("output.tga");
		// zbufferImage.write_tga_file("zbuffer.tga");
	}

	outputImage.write_tga_file("output.tga");
	zbufferImage.write_tga_file("zbuffer.tga");

	delete model;
	return 0;
}
