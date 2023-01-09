#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#define ZBUFFER_IDX(x, y, w) (x) + (y) * (w)

#include "tgaimage.h"
#include "vectors.h"

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);

void triangle(Vec2i *pts, TGAImage &image, TGAColor color);

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color);

void triangle(Vec3f *pts, Vec3f *vts, float *zbuffer, TGAImage &image,
			  TGAImage &tex, float intensity);

#endif // __TRIANGLE_H__
