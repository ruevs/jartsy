#include "entities.h"

void TriangleMesh::CalclulateNormals(bool normalize) {
    for(int i = 0; i < nTri; ++i) {
        // e0 = v1 - v0; e1 = v2 - v0; n = cross(e0, e1)
        Vector e0 = p[v[i + 1]] - p[v[i]];
        if(normalize) {
            n[i] = e0.Cross(p[v[i + 2]] - p[v[i]]).WithMagnitude(1);
        }
        else {
            n[i] = e0.Cross(p[v[i + 2]] - p[v[i]]);
        }
    }
}

inline bool TriangleMesh::IntersectTriangle(Ray &r, int tri) const {
    const Float rnProj = r.d.Dot(n[tri]);
    if(0 > rnProj) { // Triangle plane and ray are not parallel, ray is "outside"
        const Vector vr[3] = {p[v[tri]], p[v[tri + 1]], p[v[tri + 2]]};
        const Float rpDist = n[tri].Dot(vr[0]-r.o);
        if(0 > rpDist) {    // The ray is pointing to the plane (not away from it)
            const Float rt = rpDist / rnProj;
            const Point p  = r.o + rt * r.d; // Ray-plane intersection point
            for(int i = 0; i < 3; ++i) {
                // Positive means p "on the left of" vr[i]
                if(0 > n[tri].Dot((vr[(i + 1) % 3] - vr[i]).Cross(p - vr[i]))) {
                    return false;
                }
            }
            if((rt < r.tInt) && (rt > r.mintInt)) {
                r.tInt = rt; // Remember the intersection distance if it is the closest
                // PAR@@@@ Here we should fill in some structure that describes the
                // intersection - which entity, from which side etc...
            }
            return true;
        }
    } // else intersecting from inside? Refraction etc... PAR@@@@
    return false;
}

bool TriangleMesh::Intersect(Ray &r) const {
    bool ret = false;
    for(int i = 0; i < nTri; ++i) {
        ret |= IntersectTriangle(r, i);
    }
    return ret;
}