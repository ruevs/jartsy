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