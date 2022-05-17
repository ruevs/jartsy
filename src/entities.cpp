#include "entities.h"

void TriangleMesh::CalclulateNormals(bool normalize) {
    jartsyassert(p, "is a NULL pointer in a triangle mesh entity");
    jartsyassert(v, "is a NULL pointer in a triangle mesh entity");
    jartsyassert(n, "is a NULL pointer in a triangle mesh entity");
    jartsyassert(material, "is a NULL pointer in a triangle mesh entity");

    for(int i = 0; i < nTri; ++i) {
        // e0 = v1 - v0; e1 = v2 - v0; n = cross(e0, e1)
        Vector e0 = p[v[3 * i + 1]] - p[v[3 * i]];
        if(normalize) {
            n[i] = e0.Cross(p[v[3 * i + 2]] - p[v[3 * i]]).WithMagnitude(1);
        } else {
            n[i] = e0.Cross(p[v[3 * i + 2]] - p[v[3 * i]]);
        }
    }

    if(true == material->smoothShading) {
        // Calculate vertex normals
        jartsyassert(vn, "is a NULL pointer in a triangle mesh entity");

        for(int vert = 0; vert < nPoints; ++vert) {
            Vector triangleNormaSum{};
            for(int tri = 0; tri < nTri; ++tri) {
                if((vert == v[3 * tri]) || (vert == v[3 * tri + 1]) || (vert == v[3 * tri + 2])) {
                    // The vertex is pat of the triangle. Add the triangle normal to the average
                    triangleNormaSum += n[tri];
                }
            }
            vn[vert] = triangleNormaSum.WithMagnitude(1);
        }
    }
}

inline bool TriangleMesh::IntersectTriangle(Ray &r, int tri, Intersection *ints) const {
    const Float rnProj = r.d.Dot(n[tri]);
    if(0 > rnProj) { // Triangle plane and ray are not parallel, ray is "outside"
#define vr(i) (p[v[3 * tri + (i)]])     // Ugly profiling guided optimization. Saves about 10%.
        const Float rpDist = n[tri].Dot(vr(0) - r.o);
        if(0 > rpDist) {    // The ray is pointing to the plane (not away from it)
            const Float rt = rpDist / rnProj;
            // The potential intersection distance known at this point so calculate
            // the intersection only if it will be the closest on the ray.
            if((rt < r.tInt) && (rt > r.mintInt)) {
                const Point ip = r.o + rt * r.d; // Ray-plane intersection point
                for(int i = 0; i < 3; ++i) {
                    // Positive means p "on the left of" vr[i]
                    if(0 > n[tri].Dot((vr((i + 1) % 3) - vr(i)).Cross(ip - vr(i)))) {
                        return false;
                    }
                }
                r.tInt = rt; // Remember the intersection distance
                // PAR@@@@ Here we fill in a structure that describes the
                // intersection - which entity, from which side etc...
                ints->ip = ip;
                if(material->smoothShading) {
                    // Calculate and interpolated normal based on the vertex normals.
                    Float triArea = (vr(1) - vr(0)).Cross(vr(2) - vr(0)).Magnitude();
                    Float uc      = (ip - vr(0)).Cross(vr(2) - vr(0)).Magnitude() / triArea;
                    Float vc      = (vr(1) - vr(0)).Cross(ip - vr(0)).Magnitude() / triArea;

                    ints->uc = uc;
                    ints->vc = vc;
                    ints->n = vn[v[3 * tri + 1]] * uc +
                              vn[v[3 * tri + 2]] * vc +
                              vn[v[3 * tri]] * (1 - uc - vc);
                } else {
                    ints->n = n[tri];
                }
                ints->material = material;
                return true;
            }
        }
    } // else intersecting from inside? Refraction etc... PAR@@@@
    return false;
}

bool TriangleMesh::Intersect(Ray &r, Intersection *ints) const {
    bool ret = false;
    for(int i = 0; i < nTri; ++i) {
        ret |= IntersectTriangle(r, i, ints);
    }
    return ret;
}

bool Sphere::Intersect(Ray &r, Intersection *ints) const {
    bool ret                = false;
    const Vector oc         = r.o - center;
    const auto a            = r.d.MagSquared();
    const auto half_b       = oc.Dot(r.d);
    const auto c            = oc.MagSquared() - radius * radius;
    const auto discriminant = half_b * half_b - a * c;

    if(discriminant < 0)
        return false;

    const auto sqrtd = sqrt(discriminant);

    // Check if one of the roots is the closest intersection
    auto root = (-half_b - sqrtd) / a;
    if(root < r.mintInt || r.tInt < root) {
        root = (-half_b + sqrtd) / a;
        if(root < r.mintInt || r.tInt < root)
            return false;
    }

    r.tInt         = root;
    ints->ip       = r.o + r.d * r.tInt;
    ints->n        = (ints->ip - center) / radius;
    ints->material = material;
    // PAR@@@ these a meaningless until I assign a transformation to the sphere.
    // but they will be needed for texture mapping
    //    ints->uc = uc;
    //    ints->vc = vc;

    return true;
}
