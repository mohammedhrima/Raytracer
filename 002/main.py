#!/usr/bin/env python
import math


class Vector:
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"

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

    # left mul: number * vector
    def __mul__(self, other):
        # it should not be Vector * Vector, that's way we added assert
        assert not isinstance(other, Vector)
        return Vector(self.x * other, self.y * other, self.z * other)

    # right mul
    def __rmul__(self, other):
        # call left mul
        return self.__mul__(other)

    def __truediv__(self, other):
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


class Color(Vector):
    @classmethod
    def from_hex(cls, hexcolor = "#000000"):
        x = int([hexcolor[1:3]], 16)
        y = int(hexcolor[3: 5], 16)
        z = int(hexcolor[5: 6], 16)
        return cls(x, y, z)

class Point(Vector):
    pass

class Sphere:
    def __init__(self, center, radius, material) :
        self.center = center
        self.radius = radius
        self.material = material
    def intersects(self, ray):
        # check if the ray intersects this sphere
        # then return distance form intersections to center
        # else return None
        sphere_to_ray = ray.origin - self.center
        b = 2 * ray.direction.dot_product(sphere_to_ray)
        c = sphere_to_ray.dot_product(sphere_to_ray) - self.radius * self.radius
        discriminant = b*b - 4 * c
        
        if discriminant >= 0:
            dist = (-b - math.sqrt(discriminant)) / 2
            if dist > 0:
                return dist
        return None

def main():
    WIDTH = 320
    HEIGHT = 200
    camera = Vector(0, 0, -1)
    objects = [Sphere(Point(0,0,0),0.5), Color.from_hex("#FF0000")]
    scene = Scene(camera, objects, WIDTH, HEIGHT)
    engine = RenderEngine()
    image = engine.render(scene)

    with open("test.ppm", "w") as img_file:
        image.write_ppm(img_file)

if __name__ == "__main__":
    main()
