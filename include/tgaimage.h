#pragma once
#include <cstdint>
#include <fstream>
#include <vector>

#pragma pack(push, 1)
struct TGAHeader {
	std::uint8_t idlength{};
	std::uint8_t colormaptype{};
	std::uint8_t datatypecode{};
	std::uint16_t colormaporigin{};
	std::uint16_t colormaplength{};
	std::uint8_t colormapdepth{};
	std::uint16_t x_origin{};
	std::uint16_t y_origin{};
	std::uint16_t width{};
	std::uint16_t height{};
	std::uint8_t bitsperpixel{};
	std::uint8_t imagedescriptor{};
};
#pragma pack(pop)

struct TGAColor {
	std::uint8_t bgra[4] = {0, 0, 0, 0};
	std::uint8_t bytespp = {0};

	TGAColor() = default;
	TGAColor(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B,
			 const std::uint8_t A = 255)
		: bgra{B, G, R, A}, bytespp(4) {}
	TGAColor(const std::uint8_t v) : bgra{v, 0, 0, 0}, bytespp(1) {}
	TGAColor(const std::uint8_t *p, const std::uint8_t bpp) : bytespp(bpp) {
		for (int i = bpp; i--; bgra[i] = p[i])
			;
	}
	std::uint8_t &operator[](const int i) { return bgra[i]; }
	TGAColor operator*(float intensity) const {
		TGAColor res = *this;
		intensity =
			(intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
		for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * intensity;
		return res;
	}
};

struct TGAImage {
	enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

	TGAImage() = default;
	TGAImage(const int w, const int h, const int bpp);
	bool read_tga_file(const std::string filename);
	bool write_tga_file(const std::string filename, const bool vflip = true,
						const bool rle = true) const;
	void flip_horizontally();
	void flip_vertically();
	TGAColor get(const int x, const int y) const;
	void set(const int x, const int y, const TGAColor &c);
	int width() const;
	int height() const;

  private:
	bool load_rle_data(std::ifstream &in);
	bool unload_rle_data(std::ofstream &out) const;

	int w = 0;
	int h = 0;
	int bpp = 0;
	std::vector<std::uint8_t> data = {};
};
