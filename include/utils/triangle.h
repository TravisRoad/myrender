#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#define ZBUFFER_IDX(x, y, w) (x) + (y) * (w)

#include "tgaimage.h"
#include "vectors.h"

void triangle(Vec3f *pts, Vec3f *vts, Vec3f *normals, float *zbuffer,
			  TGAImage &image, TGAImage &tex, Vec3f light_dir);

#endif // __TRIANGLE_H__
