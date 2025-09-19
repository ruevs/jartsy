#ifndef JARTSY_RAY_H
#define JARTSY_RAY_H

#include "jartsyla.h"

class Ray {
public:
    Point o;      // Origin
    Vector d;     // Direction
    Float t=0;    // Scene time of the ray
    Float mintInt = 0; // Minimum "Time" along the ray for intersection (e.g. avoid close to camera).
    Float tInt = std::numeric_limits<Float>::max(); // "Time" along the ray of its intersection with the closest object.
};

inline Ray RayFromTo(Point from, Point to, Float time = 0, Float mintInt = 0,
                     Float tInt = std::numeric_limits<Float>::max()) {
    const Vector d = to - from;
    return {from, d.WithMagnitude(1), time, mintInt, tInt};
}

#endif
