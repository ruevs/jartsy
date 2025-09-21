#ifndef JARTSY_LIGHTS_H
#define JARTSY_LIGHTS_H

#include "jartsyla.h"
#include "ray.h"
#include "material.h"
#include "entities.h"

class Light {
    Vector VectorTo(const Point &from) const;
    Ray RayToLight(const Point from) const;
    bool Intersect(const Ray &r, Intersection *ints) const;
};

// An "ideal" point light source
class PointLight : Light {
public:
    Point p;
    Float intensity;
    Color c;

    PointLight(Point pt, Float i, Color col) : p(pt), intensity(i), c(col){};

    inline Vector VectorTo(const Point &from) const {
        return p - from;
    };

    Ray RayToLight(const Point from) const {
        Vector lightDir = VectorTo(from);
        // It seems to me that the "shadowBias" suggested in lecture 8 is better served
        // by a non-zero ray minimum time.
        return {from, lightDir.WithMagnitude(1.), 0., 10 * LENGTH_EPS, lightDir.Magnitude()};
    }

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
class SunLight : Light {
public:
    Vector direction;
    Float intensity;
    Color c;

    SunLight(Vector d, Float i, Color col) : direction(d), intensity(i), c(col){};

    inline Vector VectorTo(const Point &from) const {
        (void)from; // unused
        return -direction;
    };

    Ray RayToLight(const Point from) const {
        Vector lightDir = VectorTo(from);
        // It seems to me that the "shadowBias" suggested in lecture 8 is better served
        // by a non-zero ray minimum time.
        return {from, lightDir.WithMagnitude(1.), 0., 10 * LENGTH_EPS,
                std::numeric_limits<Float>::max() /* parallel light from infinity */};
    }

    bool Intersect(Ray &r, Intersection *ints) const {
        // PAR@@@@@@ Implement this!!!!
        // PAR@@@@@@ how should I fill ints->ip
        return true;
    };
};

#endif
