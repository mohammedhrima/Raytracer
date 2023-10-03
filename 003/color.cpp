#include "vector.cpp"

using Color = Vector;

void write_color(std::ostream &out, Color pixel_color)
{
    out << (int)(255.999 * pixel_color.get_x()) << " "
        << (int)(255.999 * pixel_color.get_y()) << " "
        << (int)(255.999 * pixel_color.get_z()) << "\n";
}