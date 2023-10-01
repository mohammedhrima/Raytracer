#include "Color.cpp"

int main()
{
    int w = 100;
    int h = 100;
    Image img(w, h);
    Color col(0, 0, 0);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
        {
            col.set_r((float)((i + j + 50) % 255));  // red
            col.set_g((float)((i + j + 100) % 255)); // green
            col.set_b((float)((i + j + 150) % 255)); // blue
            img.set_pixel(i, j, col);
        }
    img.write_ppm();
    // std::cout << Color::from_hex("000000") << std::endl;
}