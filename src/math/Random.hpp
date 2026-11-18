#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "math/Vec3.hpp"

struct Rng
{
    unsigned int state;

    explicit Rng(unsigned int seed = 1) : state(seed * 747796405u + 2891336453u) {}

    unsigned int next()
    {
        unsigned int old = state;
        state = state * 747796405u + 2891336453u;
        unsigned int word = ((old >> ((old >> 28u) + 4u)) ^ old) * 277803737u;
        return (word >> 22u) ^ word;
    }

    float uniform()
    {
        return (next() >> 8) * (1.0f / 16777216.0f);
    }

    float range(float min, float max)
    {
        return min + (max - min) * uniform();
    }

    Vec3 unitVector()
    {
        while (true)
        {
            Vec3 p(range(-1, 1), range(-1, 1), range(-1, 1));
            float len2 = lengthSquared(p);
            if (len2 > 1e-6f && len2 <= 1.0f)
                return p / std::sqrt(len2);
        }
    }
};

#endif
