#ifndef JARTSY_RAY_H
#define JARTSY_RAY_H

#include "jartsyla.h"

class Ray {
public:
    Point o;     // Origin (really a point but who cares)
    Vector d;    // Direction
    Float t = 0; // "Time" along the ray.
};

inline Ray RayFromTo(Point from, Point to, Float time = 0) {
    const Vector d = to - from;
    return {from, d.WithMagnitude(1), time};
}

#endif