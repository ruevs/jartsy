#ifndef JARTSY_LIGHTS_H
#define JARTSY_LIGHTS_H

#include "jartsyla.h"
#include "ray.h"
#include "material.h"
#include "entities.h"

// An "ideal" point light source
class PointLight {
public:
    Point p;
    Float intensity;
    Color c;

    inline Vector VectorTo(const Point &from) const {
        return p - from;
    };
    inline bool Intersect(const Ray &r, Intersection *ints) const {
        // Presuming that the ray direction is already normalized
        Vector rayToLight      = VectorTo(r.o);
        Float rayLightDistance = rayToLight.Magnitude();
        rayToLight             = rayToLight.WithMagnitude(1.);
        const Float dot        = r.d.Dot(rayToLight);
        if(0 <= dot && LENGTH_EPS >= 1 - dot) {
            // If the ray direction and the vector from the ray origin to the point source
            // have a positive dot product very close to one then the ray "intersects" the
            // light source.
            ints->ip = p;
            ints->n  = -rayToLight;
//            ints->material->color = c;  // PAR@@@@@@@@@ Intensity? Inverse square law?
            return true;
        }
        return false;
    };
};

// A parallel beam light coming from some direction 
// and filling all space. Like Sun light on Earth scale.
class SunLight {
public:
    Vector direction;
    Float intensity;
    Color c;

    inline Vector VectorTo(const Point &from) const {
        return -direction;
    };
    bool Intersect(Ray &r, Intersection *ints) const {

        return true;
    };
};

#endif
