#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#define ZBUFFER_IDX(x, y, w) (x) + (y) * (w)

#include "tgaimage.h"
#include "vectors.h"

void triangle(Vec3f *pts, Vec3f *vts, float *zbuffer, TGAImage &image,
			  TGAImage &tex, float intensity);

#endif // __TRIANGLE_H__
