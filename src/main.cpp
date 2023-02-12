#include "model.h"
#include "my_gl.h"
#include "tgaimage.h"
#include <iostream>
#include <limits>
#include <vector>

const int width = 800;
const int height = 800;

const Vec3f light_dir = Vec3f(1, 1, 1).normalize();
const Vec3f camera(0, 0, 10);
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

struct TexShader : public Shader {
	Vec3f varying_intensity;
	mat<2, 3, float> varying_uv;

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec2f uv = model->uv(model->face_uv(iface)[nthvert]);
		varying_uv.set_col(nthvert, uv);
		varying_intensity[nthvert] = std::max(
			0.f, model->normal(model->face_normal(iface)[nthvert]) * light_dir);
		Vec4f Vertex = embed<4>(model->vert(model->face(iface)[nthvert]));
		Vertex = Viewport * Projection * ModelView * Vertex;
		return Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor &color) {
		float intensity = varying_intensity * bar;
		Vec2f uv = varying_uv * bar;
		color = model->diffuse(uv) * intensity;
		return false;
	}
};

struct TexNormalShader : public Shader {
	mat<2, 3, float> varying_uv;

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec2f uv = model->uv(model->face_uv(iface)[nthvert]);
		varying_uv.set_col(nthvert, uv);
		Vec4f Vertex = embed<4>(model->vert(model->face(iface)[nthvert]));
		Vertex = Viewport * Projection * ModelView * Vertex;
		return Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor &color) {
		Vec2f uv = varying_uv * bar;
		Vec3f normal = model->normal(uv);
		float intensity = std::max(0.f, normal * light_dir);
		color = model->diffuse(uv) * intensity;
		// color = TGAColor(255, 255, 255) * intensity;
		return false;
	}
};

struct PhongShader : public Shader {
	mat<2, 3, float> varying_uv;
	Matrix uniform_M;
	Matrix uniform_MIT;

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec2f uv = model->uv(model->face_uv(iface)[nthvert]);
		varying_uv.set_col(nthvert, uv);
		Vec4f Vertex = embed<4>(model->vert(model->face(iface)[nthvert]));
		Vertex = Viewport * Projection * ModelView * Vertex;
		return Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor &color) {
		Vec2f uv = varying_uv * bar;
		Vec3f n =
			proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
		Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
		Vec3f r = (n * (n * l * 2.f) - l).normalize();
		float ambient = 2.0f;
		float diffuse = std::max(0.f, n * l);
		float specular =
			pow(std::max(r.z, 0.f), model->specular(uv)); // v cross r
		TGAColor c = model->diffuse(uv);
		for (int i = 0; i < 3; i++) {
			color[i] = int(std::min<float>(
				ambient + c[i] * (diffuse + 0.2 * specular), 255));
		}
		// color = TGAColor(255, 255, 255) * intensity;
		return false;
	}
};

struct TangentShader : public Shader {
	mat<2, 3, float> varying_uv;
	mat<4, 3, float> varying_tri;
	mat<3, 3, float> varying_nrm;
	mat<3, 3, float> ndc_tri;
	Matrix uniform_M;
	Matrix uniform_MIT;

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec2f uv = model->uv(model->face_uv(iface)[nthvert]);
		varying_uv.set_col(nthvert, uv);
		Vec3f nrm = model->normal(model->face_normal(iface)[nthvert]);
		varying_nrm.set_col(nthvert, proj<3>(uniform_MIT * embed<4>(nrm, 0.f)));
		Vec4f Vertex = embed<4>(model->vert(model->face(iface)[nthvert]));
		Vertex = Viewport * uniform_M * Vertex;
		varying_tri.set_col(nthvert, Vertex);
		ndc_tri.set_col(nthvert, proj<3>(Vertex / Vertex[3]));
		return Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor &color) {
		Vec3f bn = (varying_nrm * bar).normalize();
		Vec2f uv = varying_uv * bar;

		mat<3, 3, float> A;
		A[0] = ndc_tri.col(1) - ndc_tri.col(0);
		A[1] = ndc_tri.col(2) - ndc_tri.col(0);
		A[2] = bn;

		mat<3, 3, float> AI = A.invert();

		Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0],
							 varying_uv[0][2] - varying_uv[0][0], 0);
		Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0],
							 varying_uv[1][2] - varying_uv[1][0], 0);
		mat<3, 3, float> B;
		B.set_col(0, i.normalize());
		B.set_col(1, j.normalize());
		B.set_col(2, bn);

		// Vec3f n =
		// 	proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
		Vec3f n = (B * model->normal(uv)).normalize();
		Vec3f l = proj<3>(uniform_M * embed<4>(light_dir, 0.f)).normalize();
		// Vec3f r = (n * (n * l * 2.f) - l).normalize();
		// float ambient = 2.0f;
		// float diffuse = std::max(0.f, n * l);
		// float specular =
		// 	pow(std::max(r.z, 0.f), model->specular(uv)); // v cross r
		// TGAColor c = model->diffuse(uv);
		// for (int i = 0; i < 3; i++) {
		// 	color[i] = int(std::min<float>(
		// 		ambient + c[i] * (diffuse + 0.6 * specular), 255));
		// }
		color = model->diffuse(uv) * std::max(0.f, n * l);
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

	// TexNormalShader shader;
	// PhongShader shader;
	TangentShader shader;
	shader.uniform_M = Projection * ModelView;
	shader.uniform_MIT = (Projection * ModelView).invert_transpose();

	// TexShader shader;

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
