#include "model.h"
#include "tgaimage.h"
#include <iostream>

#define NUM 1

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model *model = NULL;
const int width = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror = std::abs(dy) * 2; // dy/dx * (dx * 2)
	int error = 0;
	int y = y0;
	for (int x = x0; x < x1; x++) {
		if (steep) {
			image.set(y, x, color);
		} else {
			image.set(x, y, color);
		}
		error += derror;
		if (error > dx) // 0.5 * (dx * 2)
		{
			y += (y1 - y0 > 0 ? 1 : -1);
			error -= dx * 2; // 1 * (dx * 2)
		}
	}
}

void drawModel(Model *__model, TGAImage &image) {
	for (int i = 0; i < __model->nfaces(); i++) {
		std::vector<int> face = __model->face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = __model->vert(face[j]);
			Vec3f v1 = __model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.f) * width / 2.;
			int y0 = (v0.y + 1.f) * height / 2.;
			int x1 = (v1.x + 1.f) * width / 2.;
			int y1 = (v1.y + 1.f) * height / 2.;
			line(x0, y0, x1, y1, image, white);
		}
	}
}

int main(int argc, char **argv) {
	TGAImage image(width, height, TGAImage::RGB);
	// for (int i = 0; i < NUM; i++) {
	// 	line(40, 20, 800, 900, image, red);
	// 	line(800, 400, 13, 20, image, white);
	// 	line(900, 10, 13, 400, image, red);
	// }
	// // image.flip_vertically();
	// return 0;
	model = new Model("../obj/african_head.obj");
	drawModel(model, image);
	image.write_tga_file("out.tga");

	delete model;
	return 0;
}
