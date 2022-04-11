#ifndef JARTSY_MATERIAL_H
#define JARTSY_MATERIAL_H

class Color {
    Float r, g, b;  // This is stupid. One day it should be a spectral reflectance/transmission/emittance curve
};

class Material {
public:
    Color color;
};

#endif