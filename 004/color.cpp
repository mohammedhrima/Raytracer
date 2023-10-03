#include "vector.cpp"

using Color = Vector;

void write_color(std::ostream &out, Color pixel)
{
    out << (int)(255.999 * pixel.get_x()) << " "
        << (int)(255.999 * pixel.get_y()) << " "
        << (int)(255.999 * pixel.get_z()) << "\n";
}
