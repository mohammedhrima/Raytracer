+ reflect
+ multi reflection
+ shadow

+ magnitude: lenght
+ normalize: direction

+ dot product

+ V hat = V / ||V||

+ V1 . V2 = sqrt(a1* b1 + a2 * b2 + a3 * b3)

a = 1
b = 2 * ray . sphere_to_ray
c = spher_ro_ray . sphere_to_ray - (sphere center) ^ 2
discriminant = b ^ 2 - 4 * a * c


aspectRatio ?

aspectRatio = width / height
ymax = 1  / aspectRatio
ymin = -ymax

ambient:
    shows shadows of objects

diffuse:
    take all colors and reflect it's color

specular:
    the shiny part reflected in balls (for example)


+ ray-sphere:
    ray go throw sphere  : hits 2 points
    ray intersects sphere: hits 1 points
    ray does intersects with sphere

+ sphere_to_ray = origine_of_ray - center_of_sphere 

+ video should be reviewed