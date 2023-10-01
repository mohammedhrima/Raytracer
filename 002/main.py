#!/usr/bin/env python
import math


class Vector:
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self):
        return f"(${self.x}, ${self.y}, ${self.z})"

    def dot_product(self, other):
        return self.x * other.x + self.y * other.y + self.z * other.z

    def magnitude(self):
        return math.sqrt(self.dot_product(self))

    def normalize(self):
        return self / self.magnitude()

    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other):
        return Vector(self.x - other.x, self.y - other.y, self.z - other.z)

    def __mul__(self, other):
        # it should not be Vector * Vector, that's way we added assert
        assert not isinstance(other, Vector)
        return Vector(self.x * other, self.y * other, self.z * other)

    def __rmul__(self, other):
        return self.__mul__(other)

    def __truediv__(self, other):
        # it should not be Vector * Vector, that's way we added assert
        assert not isinstance(other, Vector)
        return Vector(self.x / other, self.y / other, self.z / other)


class Image:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.pixels = [[None for _ in range(width)] for _ in range(height)]

    def set_pixel(self, x, y, col):
        self.pixels[y][x] = col

    def write_ppm(self, img_file):
        def to_byte(c):
            return round(max(min(c * 255, 255), 0))

        img_file.write("P3 {} {}\n255\n".format(self.width, self.height))
        for row in self.pixels:
            for color in row:
                img_file.write(
                    "{} {} {} ".format(
                        to_byte(color.x), to_byte(color.y), to_byte(color.z)
                    )
                )
            img_file.write("\n")


class Color (Vector):
    # store color as RGB
    @classmethod
    def from_hex(cls, hexcolor="#000000"):
        # from hexa to int, devid by 255 to get value between 0 and 1
        x = int(hexcolor[1:3], 16) / 255.0
        y = int(hexcolor[3:5], 16) / 255.0
        z = int(hexcolor[5:7], 16) / 255.0
        return cls(x, y, z)


class Point (Vector):
    pass


class Sphere:
    def __init__(self, center, radius, material):
        self.center = center
        self.radius = radius
        self.material = material

    def intersects(self, ray):
        # if ray intersects this sphere return distance to intersection
        # else return None
        sphere_to_ray = ray.origin - self.center
        a = 1
        b = 2 * ray.direction.dot_product(sphere_to_ray)
        c = sphere_to_ray.dot_product(
            sphere_to_ray) - self.radius * self.radius
        discriminant = b * b - 4*c

        if discriminant >= 0:
            dist = (-b - math.sqrt(discriminant))/2
            if dist > 0:
                return dist
        return None


class Ray:
    # ray i half line with origine and normalized direction
    def __init__(self, origin, direction):
        self.origin = origin
        self.direction = direction.normalize()


class Scene:
    # has all info for raytracing
    def __init__(self, camera, objects, width, height):
        self.camera = camera
        self.objects = objects
        self.width = width
        self.height = height


class RenderEngine:
    # render 3D objects into 2D using ray tracing
    def render(self, scene):
        width = scene.width
        height = scene.height
        aspect_ratio = float(width) / height
        x0 = -1.0
        x1 = +1.0
        xstep = (x1 - x0) / (width - 1)
        y0 = -1.0 / aspect_ratio
        y1 = +1.0 / aspect_ratio
        ystep = (y1 - y0) / (height - 1)

        camera = scene.camera
        pixels = Image(width, height)

        for j in range(height):
            y = y0 + j*ystep
            for i in range(width):
                x = x0 + i*xstep
                ray = Ray(camera, Point(x, y) - camera)
                pixels.set_pixel(i, j, self.ray_trace(ray, scene))
        return pixels

    def ray_trace(self, ray, scene):
        color = Color(0, 0, 0)
        # Find the nearest object hit by ray in the scene
        dist_hit, obj_hit = self.find_nearest(ray, scene)
        if obj_hit is None:
            return color
        hit_pos = ray.origin + ray.direction * dist_hit
        color += self.color_at(obj_hit, hit_pos, scene)
        return color

    def find_nearest(self, ray, scene):
        dist_min = None
        obj_hit = None
        for obj in scene.objects:
            dist = obj.intersects(ray)
            if dist is not None and (obj_hit is None or dist < dist_min):
                dist_min = dist
                obj_hit = obj
        return (dist_min, obj_hit)

    def color_at(self, obj_hit, hit_pos, scene):
        return obj_hit.material


def main():
    WIDTH = 320
    HEIGHT = 200
    camera = Vector(0, 0, -1)
    image = Image(WIDTH, HEIGHT)
    objects = [Sphere(Point(0, 0, 0), 0.5, Color.from_hex("#FF0000"))]
    scene = Scene(camera, objects, WIDTH, HEIGHT)
    engine = RenderEngine()
    image = engine.render(scene)

    with open("test.ppm", "w") as img_file:
        image.write_ppm(img_file=img_file)


if __name__ == "__main__":
    main()
