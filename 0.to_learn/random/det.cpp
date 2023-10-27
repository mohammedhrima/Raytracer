#include <iostream>

typedef struct
{
    float x;
    float y;
} Vec2;

// Vec2 operator+(Vec2 l, Vec2 r)
// {
//     return (Vec2){
//         l.x + r.x,
//         l.y + r.y};
// }

// Vec2 operator-(Vec2 l, Vec2 r)
// {
//     return (Vec2){
//         l.x - r.x,
//         l.y - r.y};
// }

Vec2 solve_2d_system_equation(Vec2 u, Vec2 d, Vec2 s)
{
    // det
    float dt = u.x * d.y - u.y * d.x;

    // x
    float dx = s.x * d.y - u.y * s.y;
    float x = dx / dt;

    // y
    float dy = u.x * s.y - d.x * s.x;
    float y = dy / dt;

    return (Vec2){x, y};
}

int main(void)
{
    Vec2 u = (Vec2){5, 2};
    Vec2 d = (Vec2){3, 4};
    Vec2 s = (Vec2){9, 11};
    Vec2 res = solve_2d_system_equation(u, d, s);

    std::cout << "solution: (" << res.x << "," << res.y << ")" << std::endl;
}