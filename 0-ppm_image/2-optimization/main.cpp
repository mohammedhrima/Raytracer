#include "coor.cpp"

typedef enum
{
    Reflectif_ = 11,
    Refractif_,
    Absorb_
} Mat;

class Sphere
{
public:
    Coor center;
    double radius;
    Color color;
    Mat type;

    Sphere() {}
    Sphere(Coor center_, double radius_, Color color_, Mat type_)
    {
        center = center_;
        radius = radius_;
        color = color_;
        type = type_;
    };
    bool hitSphere(Ray &ray, double tmin, double tmax, Coor &point, Coor &normal, bool &front_face, double &t)
    {
        Coor oc = ray.org - center;
        double a = ray.dir.length_squared();
        double half_b = dot(oc, ray.dir);
        double c = oc.length_squared() - radius * radius;
        double delta = half_b * half_b - a * c;

        if (delta < 0.0)
            return false;

        t = (-half_b - sqrt(delta)) / a;
        if (t <= tmin || t >= tmax)
            t = (-half_b + sqrt(delta)) / a;
        if (t <= tmin || t >= tmax)
            return false;

        point = ray.at(t);
        normal = (point - center) / radius;
        front_face = dot(ray.dir, normal) < 0;
        normal = front_face ? normal : -normal;

        return true;
    };
};

class Scene
{
public:
    Sphere *objects[100];
    int pos;

    Scene() : pos(0){};
    ~Scene(){};
    void add(Sphere *object)
    {
        objects[pos++] = object;
    }
    void clear()
    {
        for (int i = 0; i < pos; i++)
            delete objects[i];
    }
    bool hit(Ray &ray, double tmin, double tmax, Coor &point, Coor &normal, bool &front_face, Color &color, Mat &is_reflec)
    {
        // Rec tmp;
        bool did_hit = false;
        double closest = tmax;
        double t;

        for (int i = 0; i < pos; i++)
        {
            if (objects[i]->hitSphere(ray, tmin, closest, point, normal, front_face, t))
            {
                color = objects[i]->color;
                is_reflec = objects[i]->type;
                did_hit = true;
                closest = t;
            }
        }
        return did_hit;
    };
};

class Camera
{
public:
    double aspect_ratio;
    int image_width;
    int image_height;
    int max_depth;
    Coor center;
    Coor first_pixel;
    Coor pixel_u;
    Coor pixel_v;
    int samples_per_pixel;
    double vfov; // Vertical view angle (field of view)
    Coor u, v, w;
    Coor lookfrom;
    Coor lookat;
    Coor vup;

public:
    void init()
    {
        max_depth = 50;
        image_width = 800;
        aspect_ratio = 16.0 / 9.0;
        samples_per_pixel = 2;

        vfov = 90;                 // Vertical view angle (field of view)
        lookfrom = Coor(-3, 10, 2); // Point camera is looking from
        lookat = Coor(0, 0, -1);    // Point camera is looking at
        vup = Coor(0, 1, 0);       // Camera-relative "up" direction

        image_height = (image_width / aspect_ratio);
        if (image_height < 1)
            image_height = 1;
        double focal_length = (lookfrom - lookat).length();
        // TODO: to be checked
        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2);

        double viewport_height = 2.0 * h * focal_length;
        double viewport_width = viewport_height * ((double)image_width / image_height);

        // center = Coor(0, 0, 0);
        center = lookfrom;
        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // viewport vectors
        Coor viewport_u = viewport_width * u;
        Coor viewport_v = -viewport_height * v;

        // pixel to pixel step
        pixel_u = viewport_u / image_width;
        pixel_v = viewport_v / image_height;

        // upper left pixel
        Coor viewport_upper_left = center - focal_length * w - viewport_u / 2 - viewport_v / 2;
        first_pixel = viewport_upper_left + 0.5 * (pixel_u + pixel_v);
    }
    Coor reflect(Coor v, Coor n)
    {
        return v - 2 * dot(v, n) * n;
    }
    Coor refract(Coor v, Coor n, float refraction_ratio)
    {
        Coor Perp = (v - dot(v, n) * n) * refraction_ratio;
        Coor Para = -n * sqrt(1 - pow(Perp.length(), 2));
        return Para + Perp;
    }

    // TODO : to be verified
    double reflectance(double cosine, double ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        double r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

    Color ray_color(Ray &ray, int depth, Scene &world)
    {
        if (depth <= 0)
            return Color(0, 0, 0);

        Color reflcol;
        Mat mat_type;
        Coor point;
        Coor normal;
        bool front_face;

        if (world.hit(ray, 0.0001f, DBL_MAX, point, normal, front_face, reflcol, mat_type))
        {
            Ray reflray;
            if (mat_type == Reflectif_)
            {
                Coor reflected = reflect(unit_vector(ray.dir), unit_vector(normal));
#if 0
                // shadowed reflection
                reflray = Ray(point, reflected + 0.5 * random_unit_vector());
#else
                // clear reflection
                reflray = Ray(point, reflected);
#endif
                return reflcol * ray_color(reflray, depth - 1, world);
            }
            if (mat_type == Absorb_)
            {
                Coor scatt = normal + random_unit_vector();
#if 0          
                // TODO: to be verified
                // check if scatt is near 0
                double s = 1e-8;
                if (fabs(scatt.x) < s && fabs(scatt.y) < s && fabs(scatt.z) < s)
                    scatt = rec.normal;
#endif
                reflray = Ray(point, scatt);
                return reflcol * ray_color(reflray, depth - 1, world);
            }
            // TODO: to be verified
            if (mat_type == Refractif_)
            {
                double index_of_refraction = 1.5;
                double refraction_ratio = front_face ? (1.0 / index_of_refraction) : index_of_refraction;

                // Coor unit_dir = unit_vector(ray.dir);
                double cos_theta = dot(ray.dir, normal) / (ray.dir.length() * normal.length()); // fmin(dot(-unit_dir, normal), 1.0);
                double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

                Coor direction;
                if (refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_double())
                    direction = reflect(unit_vector(ray.dir), unit_vector(normal));
                else
                    direction = refract(unit_vector(ray.dir), unit_vector(normal), refraction_ratio);
                reflray = Ray(point, direction);
                return reflcol * ray_color(reflray, depth - 1, world);
            }
            return reflcol;
        }

        Coor unit = unit_vector(ray.dir);
        double a = 0.5 * (unit.y + 1.0);
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }
    void render(Scene &world)
    {
        init();
        std::cout << "P3\n";
        std::cout << image_width << ' ' << image_height << "\n255\n";
        for (int h = 0; h < image_height; ++h)
        {
            std::clog << "\rScanlines remaining: " << (image_height - h) << ' ' << std::flush;
            for (int w = 0; w < image_width; ++w)
            {
                Color pixel;
                for (int i = 0; i < samples_per_pixel; i++)
                {
                    Coor p = random_double(-0.5, 0.5) * pixel_u + random_double(-0.5, 0.5) * pixel_v;
                    Coor pixel_center = first_pixel + (w * pixel_u) + (h * pixel_v);
                    p = pixel_center + p;
                    Coor dir = p - center;
                    Ray ray(center, dir);
                    pixel += ray_color(ray, max_depth, world);
                }
                pixel /= samples_per_pixel;
                write_color(pixel);
            }
        }
        std::clog << "\rDone.                 \n";
    }
};

int main()
{
    // World
    Scene world;

    world.add(new Sphere(Coor(0.0, -100.5, -1.0), 100.0, Color(0.8, 0.8, 0.0), Absorb_));
    world.add(new Sphere(Coor(0.0, 0.0, -1.0), 0.5, Color(0.1, 0.2, 0.5), Absorb_));     // center
    world.add(new Sphere(Coor(-1.0, 0.0, -1.0), 0.5, Color(1.0, 1.0, 1.0), Refractif_)); // left
    world.add(new Sphere(Coor(1.0, 0.0, -1.0), 0.5, Color(0.8, 0.6, 0.2), Reflectif_));  // right

    Camera cam;

    cam.vfov = 90;
    cam.lookfrom = Coor(-2, 2, 1);
    cam.lookat = Coor(0, 0, -1);
    cam.vup = Coor(0, 1, 0);

    cam.render(world);
    world.clear();
}
