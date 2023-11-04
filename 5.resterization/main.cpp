#include "utils.cpp"

typedef enum
{
    none_,
    edge_,
    inside_,
} part;

part did_hit(Vec3 A, Vec3 B, Vec3 C, Vec3 P)
{
    Vec3 AB = B - A;
    Vec3 AC = C - A;
    Vec3 AP = P - A;

    float det_ = AB.x * AC.y - AB.y * AC.x;
    if (det_ == 0.0)
        return none_;
    float detx = (AP.x * AC.y - AP.y * AC.x) / det_;
    float dety = (AP.y * AB.x - AP.x * AB.y) / det_;
    if (detx < 0 || dety < 0 || detx + dety > 1)
        return none_;
    if (detx + dety == 1 || detx == 0 || dety == 0)
        return edge_;
    return inside_;
}

int main()
{
    Win *win = new_window(WIDTH, HEIGHT, (char *)"Resterization");
    Vec3 vertexes[][3] = {
        {{0, 0, 0}, {40, 0, 0}, {-40, 0, 0}},
        {{40, -40, 0}, {40, 0, 0}, {0, -40, 0}},
        
        {{0, 0, 0}, {40, 0, 0}, {0, -40, 0}},
        {{0, 0, 0}, {-40, 0, 0}, {0, -40, 0}},

        {{0, 0, 0}, {40, 40, 0}, {-40, 40, 0}},
        {{0, 80, 0}, {-40, 40, 0}, {40, 40, 0}},

        {{0, 0, 0}, {40, 0, 0}, {40, 40, 0}},
        {{0, 0, 0}, {-40, 0, 0}, {-40, 40, 0}},

        {{0, 0, 0}, {40, 0, 0}, {0, -40, 0}},
        {{0, 0, 0}, {-40, 0, 0}, {0, -40, 0}},
        
        {{0, 0, 0}, {40, 40, 0}, {-40, 40, 0}},
        {{0, 80, 0}, {-40, 40, 0}, {40, 40, 0}}
        };

    for (int i = 0; i < win->width * win->height; i++)
        win->pixels[i] = COLOR(255, 255, 255);

    for (int h = 0; h < win->height; h++)
    {
        for (int w = 0; w < win->width; w++)
        {
            for (int i = 0; i < 6; i++)
            {
                Vec3 *tri = vertexes[i];

                Vec3 P = {(float)(w - win->width / 2), (float)(win->height / 2 - h), .0f};
                Vec3 A = vertexes[i][0];
                Vec3 B = vertexes[i][1];
                Vec3 C = vertexes[i][2];
                switch (did_hit(A, B, C, P))
                {
                case inside_:
                    win->pixels[h * win->width + w] = COLOR(255, 0, 0);
                    break;
                case edge_:
                    win->pixels[h * win->width + w] = COLOR(0, 0, 255);
                    break;
                default:
                    break;
                }
            }
        }
    }
    update_window(win);
    while (1)
    {
        listen_on_events(win);
    }
}