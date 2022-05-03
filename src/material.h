#ifndef JARTSY_MATERIAL_H
#define JARTSY_MATERIAL_H

class Color {
    Float r, g, b;  // This is stupid. One day it should be a spectral reflectance/transmission/emittance curve
};

class Material {
public:
    Color indexOfRefraction;    // It is wavelength dependent
    Color reflectance;          // The surface color of the material
    Color transmission;         // The transmission of a trtansparent material
    Float surfaceSmoothness;    // 1 = perfectly flat (mirror), 0 = perfectly diffuse PAR@@?
    // For the crtscene format - controls whether flat or smooth shading is used for triangle meshes
    bool smoothShading = false;
};

#endif