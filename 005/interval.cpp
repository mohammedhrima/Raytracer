#include "sphere.cpp"

#ifndef INTERVAL
#define INTERVAL

class Interval
{
public:
    double min;
    double max;

    Interval();
    Interval(const double _min, const double _max);
    bool contains(double x) const;
    bool surrounds(double x) const;

    static const Interval empty;
    static const Interval universe;
};

Interval::Interval() : min(+infinity), max(-infinity) {}
Interval::Interval(const double _min, const double _max) : min(_min), max(_max) {}
bool Interval::contains(double x) const
{
    return x <= max && x >= min;
}
bool Interval::surrounds(double x) const
{
    return x < max && x > min;
}
const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);

#endif