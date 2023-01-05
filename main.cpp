#include <iostream>
#include "tgaimage.h"

#define NUM 1

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror = std::abs(dy) * 2; // dy/dx * (dx * 2)
    int error = 0;
    int y = y0;
    for (int x = x0; x < x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
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

int main(int argc, char **argv)
{
    TGAImage image(1000, 1000, TGAImage::RGB);
    for (int i = 0; i < NUM; i++)
    {
        line(40, 20, 800, 400, image, red);
        line(800, 400, 13, 20, image, white);
    }
    // image.flip_vertically();
    image.write_tga_file("out.tga");
    return 0;
}
