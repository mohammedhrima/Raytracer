#include "ray.cpp"

Color ray_color(const Ray &r)
{
    Vector unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.get_y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main(void)
{
    // Image
    double aspect_ratio = 16.0 / 9.0;
    int width = 400;
    int height = (int)(width / aspect_ratio);
    height = height < 1 ? 1 : height;

    // Camera
    double focal_length = 1.0;
    double view_height = 2.0;
    double view_width = view_height * aspect_ratio;
    // double view_width = view_height * (double)(width / height);
    Point camera_center = Point(0, 0, 0);

    // screen
    Vector screen_u = Vector(view_width, 0, 0);
    Vector screen_v = Vector(0, -view_height, 0);

    //
    Vector pixel_delta_u = screen_u / width;
    Vector pixel_delta_v = screen_v / height;

    //
    Vector screen_upper_left = camera_center - Vector(0, 0, focal_length) - screen_u / 2 - screen_v / 2;
    Vector pixel00_loc = screen_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render
    std::cout << "P3\n"
              << width << " " << height << "\n255\n";
    for (int h = 0; h < height; h++)
    {
        std::clog << "\rScaling remaining: " << (height - h) << " " << std::flush;
        for (int w = 0; w < width; w++)
        {
            auto pixel_center = pixel00_loc + (w * pixel_delta_u) + (h * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            Ray ray(camera_center, ray_direction);

            Color pixel_color = ray_color(ray);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\nDone         \n";
}