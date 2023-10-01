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

    # right mu l
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
    pass


def main():
    WIDTH = 3
    HEIGHT = 2
    img = Image(WIDTH, HEIGHT)
    red = Color(x=1, y=0, z=0)
    green = Color(x=0, y=1, z=0)
    blue = Color(x=0, y=0, z=1)
    img.set_pixel(0, 0, red)
    img.set_pixel(1, 0, green)
    img.set_pixel(2, 0, blue)

    img.set_pixel(0, 1, red+blue)
    img.set_pixel(1, 1, green + blue + green)
    img.set_pixel(2, 1, red * 0.001)

    with open("test.ppm", "w") as img_file:
        img.write_ppm(img_file)

if __name__ == "__main__":
    main()
