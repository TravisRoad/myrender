#include "triangle.h"

Vec3f barycentric(Vec2i *pts, Vec2i P) {
	Vec3f vx(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x);
	Vec3f vy(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
	Vec3f u = cross(vx, vy);
	/* `pts` and `P` has integer value as coordinates
	so `abs(u[2])` < 1 means `u.z` is 0, this happens
	when three point locate in the same line */
	if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);

	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

Vec3f barycentric(Vec3f *pts, Vec3f P) {
	Vec3f vx(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x);
	Vec3f vy(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
	Vec3f u = cross(vx, vy);
	/* this happens when three point locate in the same line */
	if (std::abs(u.z) == 0.f) return Vec3f(-1, 1, 1);

	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
	Vec3f s[2];
	for (int i = 2; i--;) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is
							   // zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it
							// will be thrown away by the rasterizator
}

void triangle(Vec2i *pts, TGAImage &image, TGAColor color) {
	Vec2i bboxmin(image.width() - 1, image.height() - 1);
	Vec2i bboxmax(0, 0);
	Vec2i clamp(image.width() - 1, image.height() - 1);
	for (int i = 0; i < 3; i++) {
		bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
		bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
	}
	Vec2i P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts, P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			image.set(P.x, P.y, color);
		}
	}
}

// void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) {
// 	Vec2f bboxmin(std::numeric_limits<float>::max(),
// 				  std::numeric_limits<float>::max());
// 	// Vec2f bboxmax(std::numeric_limits<float>::min(),
// 	// 			  std::numeric_limits<float>::min());
// 	Vec2f bboxmax(-std::numeric_limits<float>::max(),
// 				  -std::numeric_limits<float>::max());
// 	// Vec2f bboxmin(image.width() - 1, image.height() - 1);
// 	// Vec2f bboxmax(0, 0);
// 	Vec2f clamp(image.width() - 1, image.height() - 1);
// 	for (int i = 0; i < 3; i++) {
// 		bboxmin.x = std::max(0.f, std::min(bboxmin.x, pts[i].x));
// 		bboxmin.y = std::max(0.f, std::min(bboxmin.y, pts[i].y));
// 		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
// 		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
// 	}
// 	Vec3f P;
// 	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
// 		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
// 			// Vec3f bc_screen = barycentric(pts, P);
// 			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
// 			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
// 			P.z = 0;
// 			for (int i = 0; i < 3; i++) P.z += pts[i].z * bc_screen[i];
// 			if (P.z > zbuffer[int(P.x + P.y * image.width())]) {
// 				zbuffer[int(P.x + P.y * image.width())] = P.z;
// 				image.set(P.x, P.y, color);
// 			}
// 		}
// 	}
// }

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) {
	Vec2f bboxmin(std::numeric_limits<float>::max(),
				  std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(),
				  -std::numeric_limits<float>::max());
	Vec2f clamp(image.width() - 1, image.height() - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y * image.width())] < P.z) {
				zbuffer[int(P.x + P.y * image.width())] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}
}

void triangle(Vec3f *pts, Vec3f *vts, float *zbuffer, TGAImage &image,
			  TGAImage &tex, float intensity) {
	Vec2f bboxmin(std::numeric_limits<float>::max(),
				  std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(),
				  -std::numeric_limits<float>::max());
	Vec2f clamp(image.width() - 1, image.height() - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];

			TGAColor color;

			Vec3f vtP(0., 0., 0.);
			for (int i = 0; i < 3; i++) {
				Vec3f uv(vts[i].x * tex.width(), vts[i].y * tex.height(), 0.);
				vtP = vtP + uv * bc_screen[i];
			}
			color = tex.get(int(vtP.x), int(vtP.y));
			for (int i = 0; i < 4; i++) {
				color[i] *= intensity;
			}

			if (zbuffer[int(P.x + P.y * image.width())] < P.z) {
				zbuffer[int(P.x + P.y * image.width())] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}
}
