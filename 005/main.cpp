#include "coor.cpp"

typedef enum
{
    Reflectif_ = 11,
    Refractif_,
    Absorb_
} Mat;

class Rec
{
public:
    Coor point;
    Coor normal;
    double t;
    bool front_face;

    void set_face_normal(Ray &ray, Coor &norm)
    {
        front_face = dot(ray.dir, norm) < 0;
        normal = front_face ? norm : -norm;
    }
};

class Sphere
{
public:
    Coor center;
    double radius;
    Color color;
    Mat type;

    Sphere()
    {
    }
    Sphere(Coor center_, double radius_, Color color_, Mat type_)
    {
        center = center_;
        radius = radius_;
        color = color_;
        type = type_;
    };
    bool hit(Ray &ray, double tmin, double tmax, Rec &rec)
    {
        Coor oc = ray.org - center;
        double a = ray.dir.length_squared();
        double half_b = dot(oc, ray.dir);
        double c = oc.length_squared() - radius * radius;
        double delta = half_b * half_b - a * c;

        if (delta < 0.0)
            return false;

        double t = (-half_b - sqrt(delta)) / a;
        if (t <= tmin || t >= tmax)
            t = (-half_b + sqrt(delta)) / a;
        if (t <= tmin || t >= tmax)
            return false;

        rec.t = t;
        rec.point = ray.at(t);
        Coor norm = (rec.point - center) / radius;
        rec.set_face_normal(ray, norm);

        return true;
    };
};

class Scene
{
public:
    Sphere *objects[100];
    int pos;

    Scene()
    {
        pos = 0;
    };
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
    bool hit(Ray &ray, double tmin, double tmax, Rec &rec, Color &color, Mat &is_reflec)
    {
        Rec tmp;
        bool did_hit = false;
        double closest = tmax;

        for (int i = 0; i < pos; i++)
        {
            if (objects[i]->hit(ray, tmin, closest, tmp))
            {
                color = objects[i]->color;
                is_reflec = objects[i]->type;
                did_hit = true;
                closest = tmp.t;
                rec = tmp;
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

public:
    void init()
    {
        max_depth = 10;

        image_width = 800;
        aspect_ratio = 16.0 / 9.0;
        samples_per_pixel = 32;
        image_height = (image_width / aspect_ratio);
        if (image_height < 1)
            image_height = 1;
        double focal_length = 1.0;
        double viewport_height = 2.0;
        double viewport_width = viewport_height * ((double)image_width / image_height);

        center = Coor(0, 0, 0);
        // viewport vectors
        Coor viewport_u = Coor(viewport_width, 0, 0);
        Coor viewport_v = Coor(0, -viewport_height, 0);

        // pixel to pixel step
        pixel_u = viewport_u / image_width;
        pixel_v = viewport_v / image_height;

        // upper left pixel
        Coor viewport_upper_left = center - Coor(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        first_pixel = viewport_upper_left + 0.5 * (pixel_u + pixel_v);
    }
    Coor reflect(Coor v, Coor n)
    {
        return v - 2 * dot(v, n) * n;
    }

    // TODO: to be verified
    Coor refract(const Coor &uv, const Coor &n, double etai_over_etat)
    {
        double cos_theta = fmin(dot(-uv, n), 1.0);
        Coor r_out_perp = etai_over_etat * (uv + cos_theta * n);
        Coor r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
        return r_out_perp + r_out_parallel;
    }

    /*
    absorb:
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& reflray){
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;

            reflray = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    Reflect:
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered){
            Coor reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected);
            attenuation = albedo;
            return true;
        }

    Refract:
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered){
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract)
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);
    }

    */

    Color ray_color(Ray &ray, int depth, Scene &world)
    {
        if (depth <= 0)
            return Color(0, 0, 0);

        Rec rec;
        Color reflcol;
        Mat mat_type;
        if (world.hit(ray, 0.0001f, DBL_MAX, rec, reflcol, mat_type))
        {
            Ray reflray;
            if (mat_type == Reflectif_)
            {
                Coor reflected = reflect(unit_vector(ray.dir), rec.normal);
#if 0
                // shadowed reflection
                reflray = Ray(rec.point, reflected + 0.5 * random_unit_vector());
#else
                // clear reflection
                reflray = Ray(rec.point, reflected);
#endif
                return reflcol * ray_color(reflray, depth - 1, world);
            }
            if (mat_type == Absorb_)
            {
                Coor scatt = rec.normal + random_unit_vector();
#if 0          
                // TODO: to be verified
                // check if scatt is near 0
                double s = 1e-8;
                if (fabs(scatt.x) < s && fabs(scatt.y) < s && fabs(scatt.z) < s)
                    scatt = rec.normal;
#endif
                reflray = Ray(rec.point, scatt);
                return reflcol * ray_color(reflray, depth - 1, world);
            }
            // TODO: to be verified
            if (mat_type == Refractif_)
            {
                Color atten = Color(1, 1, 1);
                double index_of_refraction = 1.5;
                double refraction_ratio = rec.front_face ? (1.0 / index_of_refraction) : index_of_refraction;

                Coor unit_dir = unit_vector(ray.dir);
                double cos_theta = fmin(dot(-unit_dir, rec.normal), 1.0);
                double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

                bool cannot_refract = refraction_ratio * sin_theta > 1.0;
                Coor direction;

                if (cannot_refract)
                    direction = reflect(unit_dir, rec.normal);
                else
                    direction = refract(unit_dir, rec.normal, refraction_ratio);

                reflray = Ray(rec.point, direction);
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
    /*
    Reflectif_ = 11,
        Absorb_*/
    world.add(new Sphere(Coor(0, -100.5, -1), 100, Color(0.8, 0.8, 0.0), Absorb_));
    world.add(new Sphere(Coor(0.0, 0.0, -1.0), 0.5, Color(0.7, 0.3, 0.3), Absorb_));
    world.add(new Sphere(Coor(-1.0, 0.0, -1.0), 0.5, Color(0.8, 0.8, 0.8), Refractif_));
    world.add(new Sphere(Coor(1.0, 0.0, -1.0), 0.5, Color(0.8, 0.6, 0.2), Reflectif_));

    Camera cam;

    cam.render(world);
    world.clear();
}
