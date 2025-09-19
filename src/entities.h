#ifndef JARTSY_ENTITIES_H
#define JARTSY_ENTITIES_H

#include "jartsyla.h"
#include "ray.h"
#include "material.h"

// Carries the information about the intersection between a ray and an entity
class Intersection {
public:
    Point ip;
    Normal n;   // This is not enough - e.g. Phong shading... dn/du, dn/dv ?
    Float uc, vc;
    Material *material;
    // PAR@@@@ Entity reference? Textures?
};

class TriangleMesh {
public:
    unsigned nPoints;
    unsigned nTri;
    // The triangle mesh consists of a point cloud "p". Triangles are described with an index "v"
    // into the point cloud listing indexes of the vertexes (v0, v1, v2) of each triangle in order.
    // "n" are the triangle normals for each triangle in the same oreder as the index.
    Point *p;
    Normal *vn; // Vertex normals for smooth shading - one for each point
    unsigned *v;
    Normal *n;
    Material *material;
    // PAR@@@@@@ Does something else outside manage the memory for the triangle mesh?
    // This is all very dirty and "dangerous".

    bool Intersect(Ray &r, Intersection *ints) const;
    void CalclulateNormals(bool normalize=true);

private:
    inline bool IntersectTriangle(Ray &r, unsigned tri, Intersection *ints) const;
};

class Sphere {
public:
    Point center;
    Float radius;
    Material *material;

    bool Intersect(Ray &r, Intersection *ints) const;
};

#endif
