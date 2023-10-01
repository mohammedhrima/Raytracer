#include "Vector.cpp"

class Image
{
private:
    int width;
    int height;
    Vector **pixels;

public:
    Image(int w, int h);
    ~Image();
    void set_pixel(int x, int y, Vector &color);
    void write_ppm();
};

Image::Image(int w, int h) : width(w), height(h)
{
    pixels = new Vector *[height];
    for (int i = 0; i < height; i++)
        pixels[i] = new Vector[width];
}

Image::~Image()
{
    for (int i = 0; i < height; i++)
        delete[] pixels[i];
    delete[] pixels;
}

void Image::set_pixel(int h, int w, Vector &color)
{
    pixels[h][w] = color;
}

void Image::write_ppm()
{
    // std::ofstream outFile(filename.c_str());
    std::cout << "P3 " << width << " " << height << std::endl;
    std::cout << "255" << std::endl;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
            std::cout << pixels[j][i] << " ";
        std::cout << std::endl;
    }
}