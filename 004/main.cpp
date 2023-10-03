#include "ray.cpp"

double hit_sphere(const Point &center, double radius, const Ray &ray)
{
    Vector OC = ray.origin() - center;
    // double a = dot(ray.direction(), ray.direction());
    double a = ray.direction().length_squared();
    // double b = 2.0 * dot(OC, ray.direction());
    double half_b = dot(OC, ray.direction());
    // double c = dot(OC, OC) - radius * radius;
    double c = OC.length_squared() - radius * radius;
    // double delta = b * b - a * c;
    double delta = half_b * half_b - a * c;

    if (delta < 0)
        return -1.0;
    // return ((-half_b - sqrt(delta)) / (2.0 * a));
    return ((-half_b - sqrt(delta)) / a);
}

Color ray_color(const Ray &ray)
{
    auto x1 = hit_sphere(Point(0, 0, -1), 0.5, ray);
    if (x1 > 0.0)
    {
        Vector N = unit_vector(ray.at(x1) - Vector(0, 0, -1));
        return 0.5 * Color(N.get_x() + 1, N.get_y() + 1, N.get_z() + 1);
    }
    Vector unit_direction = unit_vector(ray.direction());
    double a = 0.5 * (unit_direction.get_y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main(void)
{
    // Ratio
    double ratio = 16.0 / 9.0;

    // Image
    // TODO: try testing width = height
    int width = 400;
    int height = (int)(width / ratio);
    if (height < 1)
        height = 1;

    // Camera
    double focal_length = 1.0;
    Point camera_center(0, 0, 0);

    // Viewport
    double viewport_h = 2.0;
    double viewport_w = viewport_h * (double)width / height;

    // Viewport vectors
    Vector viewport_u(viewport_w, 0, 0);
    Vector viewport_v(0, -viewport_h, 0);

    // Viewport vertical and horizontal pixel to pixel
    Vector pixel_delta_u = viewport_u / width;
    Vector pixel_delta_v = viewport_v / height;

    // upper left pixel location
    Point viewport_upper_left = camera_center - Vector(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    Point pixel00_center = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render
    std::cout << "P3\n"
              << width << " " << height << "\n255\n";
    for (int h = 0; h < height; h++)
    {
        std::clog << "\rScanlines remaining: " << (height - h) << ' ' << std::flush;
        for (int w = 0; w < width; w++)
        {
            Point pixel_center = pixel00_center + (w * pixel_delta_u) + (h * pixel_delta_v);
            Vector ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);
            // Color pixel = Color((double)w / (width - 1), (double)h / (height - 1), 0.0);
            Color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                 \n";
}