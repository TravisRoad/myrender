#ifndef __MY_GL_H__
#define __MY_GL_H__

#include "tgaimage.h"
#include "vectors.h"

void lookat(Vec3f eye, Vec3f center, Vec3f up);
void projection(float coeff = 0.f);
void viewport(float x, float y, float w, float h);

extern Matrix Projection;
extern Matrix ModelView;
extern Matrix Viewport;

struct Shader {
  public:
	virtual ~Shader();
	virtual Vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

void triangle(Vec4f *pts, Shader &shader, TGAImage &outputImage,
			  TGAImage &zbufferImage);

#endif // __MY_GL_H__
