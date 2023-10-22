#include "utils.c"

// Ray tracing
float hit_sphere(Obj sphere, Ray ray, float min, float max)
{
    Vec3 OC = sub_vec3(ray.org, sphere.center);
    float a = length_squared(ray.dir);
    float half_b = dot(OC, ray.dir);
    float c = length_squared(OC) - sphere.radius * sphere.radius;
    float delta = half_b * half_b - a * c;
    if (delta < .0)
        return -1.0;
    float sq_delta = sqrtf(delta);
    float sol = (-half_b - sq_delta) / a;
    if (sol <= min || sol >= max)
        sol = (-half_b + sq_delta) / a;
    if (sol <= min || sol >= max)
        return -1.0;
    return (sol);
}

float hit_plan(Obj plan, Ray ray, float min, float max)
{
    float t = plan.d - dot(plan.normal, ray.org);
    float div = dot(ray.dir, plan.normal);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;
    return (t);
}
// // TODO : to be verified
// float reflectance(float cosine, float ref_idx)
// {
//     // Use Schlick's approximation for reflectance.
//     float r0 = (1 - ref_idx) / (1 + ref_idx);
//     r0 = r0 * r0;
//     return r0 + (1 - r0) * pow((1 - cosine), 5);
// }
Ray render_object(Obj obj, Ray ray, float closest)
{
    // point coordinates
    Vec3 cp_norm;
    Vec3 p = point_at(ray, closest);
    if (obj.type == sphere_)
    {
        cp_norm = unit_vector(sub_vec3(p, obj.center));
    }
    else if (obj.type == plan_ || obj.type == triangle_)
        cp_norm = obj.normal;

    bool same_dir = dot(cp_norm, ray.dir) >= 0;
    if (same_dir) // to be used when drawing triangle
        cp_norm = (Vec3){-cp_norm.x, -cp_norm.y, -cp_norm.z};
    Vec3 ranv = random_unit_vector();
    Vec3 ndir;
    if (obj.mat == Refl_)
    {
        float val;
        val = -2 * dot(ray.dir, cp_norm);
        ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
    }
    else if (obj.mat == Refr_)
    {
        float index_of_refraction = 1.5;
        float refraction_ratio = same_dir ? index_of_refraction : (1.0 / index_of_refraction);

        float cos_theta = dot(ray.dir, cp_norm) / (length(ray.dir) * length(cp_norm)); // fmin(dot(-unit_dir, normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
#if 0
        if (refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_float(-FLT_MAX, FLT_MAX))
#else
        if (refraction_ratio * sin_theta > 1.0)
#endif
        {
            // Reflect
            float val;
            val = -2 * dot(ray.dir, cp_norm);
            ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
        }
        else
        {
            // Refract
            Vec3 ray_dir = unit_vector(ray.dir);
            Vec3 Perp = mul_vec3(refraction_ratio, sub_vec3(ray_dir, mul_vec3(dot(ray_dir, cp_norm), cp_norm)));
            Vec3 Para = mul_vec3(sqrt(1 - pow(length(Perp), 2)), mul_vec3(-1, cp_norm));
            ndir = add_vec3(Perp, Para);
        }
    }
    else if (obj.mat == Abs_)
        ndir = add_vec3(cp_norm, ranv);
    return (Ray){.org = p, .dir = ndir};
}
// TODO: Barycentric Coordinates
float hit_triangle(Obj trian, Ray ray, float min, float max)
{
    float t = dot(trian.normal, sub_vec3(trian.p1, ray.org));
    float div = dot(trian.normal, ray.dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;
    Vec3 v0 = sub_vec3(trian.p2, trian.p1);
    Vec3 v1 = sub_vec3(trian.p3, trian.p1);
    Vec3 v2 = sub_vec3(point_at(ray, t), trian.p1);
    float dot00 = dot(v0, v0);
    float dot01 = dot(v0, v1);
    float dot02 = dot(v0, v2);
    float dot11 = dot(v1, v1);
    float dot12 = dot(v1, v2);
    float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    if (u < 0 || v < 0 || u + v > 1)
        return -1.0;
    return t;
}

Color ray_color(Win *win, Ray ray, int depth)
{
    Scene *scene = &win->scene;
    Color light = {};
    Color attenuation = {1, 1, 1};

    for (int bounce = 0; bounce < depth; bounce++)
    {
        float closest = FLT_MAX;
        int hit_index = -1;
        float x = .0;
        for (int i = 0; i < scene->pos; i++)
        {
            if (scene->objects[i].type == sphere_)
                x = hit_sphere(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == plan_)
                x = hit_plan(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == triangle_)
                x = hit_triangle(scene->objects[i], ray, ZERO, closest);
            if (x > .0)
            {
                hit_index = i;
                closest = x;
            }
        }
        if (hit_index != -1)
        {
            Obj *obj = &scene->objects[hit_index];
            ray = render_object(*obj, ray, closest);
            light = add_vec3(light, mul_vec3_(attenuation, mul_vec3(obj->light_intensity, obj->light_color)));
            attenuation = mul_vec3_(attenuation, obj->color);
        }
        else
        {
            float a = 0.5 * (unit_vector(ray.dir).y + 1.0);
            float r = (1.0 - a) + a * .3;
            float g = (1.0 - a) + a * .7;
            float b = (1.0 - a) + a * 1.0;

            light = add_vec3(light, mul_vec3_(attenuation, (Color){r, g, b}));
            break;
        }
        if (attenuation.x <= ZERO && attenuation.y <= ZERO && attenuation.z <= ZERO)
            break;
    }
    return light;
}

int draw(void *ptr)
{
    static int frame;
    static Color *sum;
    struct timespec time_start, time_end;
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    frame++;

    Win *win = (Win *)ptr;
    Scene *scene = &win->scene;
    if (sum == NULL)
        sum = calloc(win->width * win->height, sizeof(Color));
#pragma omp parallel for
    for (int h = 0; h < win->height; h++)
    {
        for (int w = 0; w < win->width; w++)
        {
            Vec3 pixel_center = add_vec3(add_vec3(scene->first_pixel, mul_vec3(w + random_float(0, 1), scene->pixel_u)), mul_vec3(h + random_float(0, 1), scene->pixel_v));
            Vec3 dir = sub_vec3(pixel_center, scene->camera);
            Ray ray = (Ray){.org = scene->camera, .dir = dir};
            Color pixel = ray_color(win, ray, 5);
            sum[h * win->width + w] = add_vec3(sum[h * win->width + w], pixel);
            pixel = div_vec3(sum[h * win->width + w], (float)frame);
            if (pixel.r > 1)
                pixel.r = 1;
            if (pixel.g > 1)
                pixel.g = 1;
            if (pixel.b > 1)
                pixel.b = 1;
            char *dst = win->addr + (h * win->line_length + w * (win->bits_per_pixel / 8));
            *(unsigned int *)dst = ((int)(255.999 * sqrtf(pixel.r)) << 16) | ((int)(255.999 * sqrtf(pixel.g)) << 8) | ((int)(255.999 * sqrtf(pixel.b)));
        }
    }
    mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    float frame_time = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
    printf("%6.2f: render frame %d\n", frame_time, frame);
    return 0;
}

void init(Win *win)
{
    Scene *scene = &win->scene;
    scene->objects = calloc(300000, sizeof(Obj));
    win->width = 512;
    win->height = (int)((float)win->width / 1.0);
    if (win->height < 1)
        win->height = 1;

    scene->view_angle = degrees_to_radians(60);
    scene->camera = (Vec3){0, 0, -1};
    scene->camera_dir = sub_vec3((Vec3){0.1, 0.1, 0}, scene->camera); // direction
    Vec3 rightv = (Vec3){1, 0, 0};                                    // used for getting u,v,w

    scene->len = 2; // distance from screen
    float tang = tan(scene->view_angle / 2);
    float screen_height = 2 * tang * scene->len;
    float screen_width = screen_height * ((float)win->width / win->height);

    scene->w = unit_vector(div_vec3(scene->camera_dir, scene->len)); // z+ (get w vector)
    scene->v = unit_vector(cross_(rightv, scene->w));                // y+ (get v vector)
    scene->u = unit_vector(cross_(scene->w, scene->v));              // x+ (get u vector)

    // viewport steps
    scene->screen_u = mul_vec3(screen_width, scene->u);
    scene->screen_v = mul_vec3(screen_height, scene->v);
    // window steps
    scene->pixel_u = div_vec3(scene->screen_u, win->width);
    scene->pixel_v = div_vec3(scene->screen_v, win->height);

    Vec3 screen_center = add_vec3(scene->camera, mul_vec3(scene->len, scene->w));                       // camera + len * w
    Vec3 upper_left = sub_vec3(screen_center, div_vec3(add_vec3(scene->screen_u, scene->screen_v), 2)); // center - screen_u / 2 - screen_v / 2 = center - (screen_u+screen_v)/2
    scene->first_pixel = add_vec3(upper_left, div_vec3(add_vec3(scene->pixel_u, scene->pixel_v), 2));   // upper_left + (pixel_u + pixel_v) / 2
}

typedef struct
{
    int v, vt, vn;
} FaceVertex;

void parse_cube(Scene *scene, char *name)
{
    FILE *file = fopen(name, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open the .obj file.\n");
        exit(1);
    }

    char line[128];
    Vec3 *vertices = NULL;
    Vec3 *normals = NULL;
    Vec3 *textures = NULL;
    Obj *triangles = scene->objects;
    int numVertices = 0;
    int numNormals = 0;
    int numTextures = 0;
    // scene->pos = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            Vec3 vertex;
            if (sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z) == 3)
            {
                numVertices++;
                // vertex.x += 2; 
                vertex.y += 2;  
                vertex.z += 10;
                vertices = (Vec3 *)realloc(vertices, numVertices * sizeof(Vec3));
                vertices[numVertices - 1] = vertex;
            }
        }
        else if (line[0] == 'v' && line[1] == 'n')
        {
            Vec3 normal;
            if (sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z) == 3)
            {
                numNormals++;
                normals = (Vec3 *)realloc(normals, numNormals * sizeof(Vec3));
                normals[numNormals - 1] = normal;
            }
        }
        else if (line[0] == 'v' && line[1] == 't')
        {
            Vec3 texture;
            if (sscanf(line, "vt %f %f %f", &texture.x, &texture.y, &texture.z) >= 2)
            {
                numTextures++;
                textures = (Vec3 *)realloc(textures, numTextures * sizeof(Vec3));
                textures[numTextures - 1] = texture;
            }
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            FaceVertex face[3];
            if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &face[0].v, &face[0].vt, &face[0].vn,
                       &face[1].v, &face[1].vt, &face[1].vn,
                       &face[2].v, &face[2].vt, &face[2].vn) == 9)
            {
                scene->pos++;
                // triangles = (Obj *)realloc(triangles, scene->pos * sizeof(Obj));

                for (int i = 0; i < 3; i++)
                {
                    triangles[scene->pos - 1].type = triangle_;
                    triangles[scene->pos - 1].color = (Color){1, 0, 0};
                    triangles[scene->pos - 1].normal = normals[face[i].vn - 1];
                    switch (i)
                    {
                    case 0:
                        triangles[scene->pos - 1].p1 = vertices[face[i].v - 1];
                        break;
                    case 1:
                        triangles[scene->pos - 1].p2 = vertices[face[i].v - 1];
                        break;
                    case 2:
                        triangles[scene->pos - 1].p3 = vertices[face[i].v - 1];
                        break;
                    }
                }
                // printf("new triangle: \n");
                // printf("\tp1(%f, %f, %f)\n", triangles[scene->pos - 1].p1.x, triangles[scene->pos - 1].p1.y, triangles[scene->pos - 1].p1.z);
                // printf("\tp2(%f, %f, %f)\n", triangles[scene->pos - 1].p2.x, triangles[scene->pos - 1].p2.y, triangles[scene->pos - 1].p2.z);
                // printf("\tp3(%f, %f, %f)\n", triangles[scene->pos - 1].p3.x, triangles[scene->pos - 1].p3.y, triangles[scene->pos - 1].p3.z);
            }
        }
    }

    fclose(file);

    if (numVertices == 0 || scene->pos == 0)
    {
        fprintf(stderr, "Error: No vertices or triangles found in the .obj file.\n");
        exit(1);
    }
}

int main(void)
{
    Win win = {0};
    init(&win);
#if 1
    parse_cube(&win.scene, "cube.obj");
#else
    // // add objects
    Vec3 p1, p2, p3;
    p1 = (Vec3){3, 3, 19};
    p2 = (Vec3){3, 1, 19};
    p3 = (Vec3){1, 3, 19};
    win.scene.objects[win.scene.pos++] = new_triangle(p1, p2, p3, (Color){1, 0, 0}, Abs_);
#endif

    win.mlx = mlx_init();
    win.win = mlx_new_window(win.mlx, win.width, win.height, "Mini Raytracer");
    win.img = mlx_new_image(win.mlx, win.width, win.height);
    win.addr = mlx_get_data_addr(win.img, &win.bits_per_pixel, &win.line_length, &win.endian);

    mlx_loop_hook(win.mlx, draw, &win);
    mlx_key_hook(win.win, listen, &win);
    mlx_loop(win.mlx);
}
