+ create:
    + sphere
    + cylinder
    + Cone
    - Hyperboloid
    - Paraboloid !!!

+ rotate obj form angle and unit axis https://en.wikipedia.org/wiki/Rotation_matrix#Axis_and_angle
+ lightning, parsing, cone
+ add textures
+ build simple arm

+ “Rasterization"
+ detect closest objects
+ create a mini solid works
+ start a mini game (maybe a mini pacman)

+ 42 subject:
    - check memory leaks
    - objects to render: plane, sphere, cylinder
    - ambient, defuse lighting
    - exit window by cliquing ESC or red cross button (free allocated space)
    - textures
    - parsing:
        - file.rt (file must ends with .rt)
        - elements in capital letter can be only declared once in the scene
        - A 0.2 255,255,255
            A   : identifier
            0.2 : ambient lighting ratio range ???
            RGB : color
        - C -50.0,0,20  0,0,1   70
            C    : identifier (Camera)
            x,y,z: coordinates
            dir  : 0,0,1
            fov  : field of view
        - L -40.0,50.0,0.0 0.6 10,0,255
            L    : light
            x,y,z: coordinates
            0.6  : light brightness ratio
            RGB  : light color (used only in bonus)
        - sp 0.0,0.0,20.6 12.6 10,0,255
            sp   : sphere
            x,y,z: coordinates
            diam : 12.6
            RGB  : color
        - pl 0.0,0.0,-10.0 0.0,1.0,0.0 0,0,225
            pl   : plane
            x,y,z: coordinates of a point in plane
            norm : normal
            RGB  : color
        - cy 50.0,0.0,20.6 0.0,0.0,1.0 14.2 21.42 10,0,255
            cy   : cylinder
            x,y,z: coordinates
            norm : normal
            diam : diameter
            h    : height
            RGB  : color
    - Error messaging
        + color range
        + normal range
    - Bonus:
        - specular reflection
        - color disruption ?
        - multi-spot lights ?
        - Cone, Hyperboloid, Paraboloid
        - textures

+ check forbiden function calloc ...