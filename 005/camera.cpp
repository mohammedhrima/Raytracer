#include "interval.cpp"

class Camera
{
public:
    double aspect_ratio = 1.0;
    int width = 100;
private:
    int height;
    Point camera_center;
    Point pixel00_center;
    Vector pixel_delta_u;
    Vector pixel_delta_v;

    // methods
    void initialize();
    Color ray_color(const Ray &ray, const Hittable &world) const;

public:
    void render(const Hittable &world);
};

void Camera::initialize()
{
    // Image
    // TODO: try testing width = height
    height = (int)(width / aspect_ratio);
    if (height < 1)
        height = 1;

    // Camera
    camera_center = Point(0, 0, 0);

    // Viewport
    double focal_length = 1.0;
    double viewport_h = 2.0;
    double viewport_w = viewport_h * (double)width / height;

    // Viewport vectors
    Vector viewport_u(viewport_w, 0, 0);
    Vector viewport_v(0, -viewport_h, 0);

    // Viewport vertical and horizontal pixel to pixel
    pixel_delta_u = viewport_u / width;
    pixel_delta_v = viewport_v / height;

    Point viewport_upper_left = camera_center - Vector(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    pixel00_center = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}

Color Camera::ray_color(const Ray &ray, const Hittable &world) const
{
    Hit_record rec;
    Interval val(0, infinity);
    if (world.hit(ray, val, rec))
        return 0.5 * (rec.normal + Color(1, 1, 1));

    Vector unit_direction = unit_vector(ray.direction());
    double a = 0.5 * (unit_direction.get_y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

void Camera::render(const Hittable &world)
{
    initialize();
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
            Ray ray(camera_center, ray_direction);
            // Color pixel = Color((double)w / (width - 1), (double)h / (height - 1), 0.0);
            Color pixel_color = ray_color(ray, world);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                 \n";
}