#ifndef JARTSY_ENTITIES_H
#define JARTSY_ENTITIES_H

#include "jartsyla.h"

class TriangleMesh {
public:
    int nTri;
    // The triangle mesh consists of a point cloud "p". Triangles are descriped with an index "v"
    // into the point cloud listing indexes of the vertexes (v0, v1, v2) of each triangle in order.
    // "n" are the triangle normals for each triangle in the same oreder as the index.
    const Point *p;
    const int *v;
    Normal *n;
    // PAR@@@@@@ Does something else outside manage the momory for the triangle mesh?

    void CalclulateNormals(bool normalize=true);
};

#endif