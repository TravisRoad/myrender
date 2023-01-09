#include "model.h"
#include "tgaimage.h"
#include "triangle.h"
#include <iostream>
#include <vector>

#define NUM 1
// #define DEBUG

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
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

void drawModel(Model *model, TGAImage &image, TGAImage &tex, float *zbuffer) {
	Vec3f light_dir(0, 0, -1);

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		std::vector<int> face_tex = model->face_tex(i);
		Vec3f screen_coords[3];
		Vec3f world_coords[3];
		Vec3f tex_uvs[3];
		for (int j = 0; j < 3; j++) {
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = Vec3f(int((v.x + 1.f) * width / 2.f),
									 int((v.y + 1.f) * height / 2.f), v.z);
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
			// triangle(screen_coords, zbuffer, image,
			// 		 TGAColor(intensity * 255, intensity * 255, intensity * 255,
			// 				  255));
			// Vec2i pts[3];
			// for (int i = 0; i < 3; i++) {
			// 	pts[i].x = int(screen_coords[i].x);
			// 	pts[i].y = int(screen_coords[i].y);
			// }
			// triangle(pts, image,
			// 		 TGAColor(intensity * 255, intensity * 255, intensity * 255,
			// 				  255));
			triangle(screen_coords, tex_uvs, zbuffer, image, tex, intensity);
		}
	}
}

int main(int argc, char **argv) {
	TGAImage image(width, height, TGAImage::RGB);
	TGAImage texture;
	Model model("../assets/obj/african_head.obj");

	texture.read_tga_file("../assets/tex/african_head_diffuse.tga");
	// texture.flip_horizontally();
	texture.flip_vertically();

	float *zbuffer = new float[width * height];
	for (int i = width * height - 1; i >= 0; i--)
		zbuffer[i] = -std::numeric_limits<float>::max(); // -INF

	drawModel(&model, image, texture, zbuffer);

	image.write_tga_file("out.tga");

	delete zbuffer;
	return 0;
}

Vec3f world2screen(Vec3f v) {
	return Vec3f(int((v.x + 1.) * width / 2. + .5),
				 int((v.y + 1.) * height / 2. + .5), v.z);
}
