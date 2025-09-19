#ifndef JARTSY_MATERIAL_H
#define JARTSY_MATERIAL_H

class Color {
public:
    Float r, g, b;  // This is stupid. One day it should be a spectral reflectance/transmission/emittance curve

    inline Color operator-() const {
        return {-r, -g, -b};
    }
    inline auto operator[](int i) {
        switch(i) {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        default: jartsyassert(false, "Unexpected color element index");
        }
    }
    inline auto &operator+=(const Color &c) {
        r += c.r;
        g += c.g;
        b += c.b;
        return *this;
    }
    inline auto &operator-=(const Color &c) {
        return *this += -c;
    }
    inline auto &operator*=(const Float t) {
        r *= t;
        g *= t;
        b *= t;
        return *this;
    }
    inline auto &operator/=(const Float t) {
        return *this *= (1 / t);
    }

    inline bool operator==(const Color &c) const {
        return r == c.r && g == c.g && b == c.b;
    }
    // Should this or the operator== be the exact comparison? Which one will I use more often?
    inline bool EqualsExactly(const Color &c) const {
        return r == c.r && g == c.g && b == c.b;
    };

    inline bool operator!=(const Color &c) const {
        return !(*this == c);
    }

    inline Color operator+(const Color &c) const {
        return {r + c.r, g + c.g, b + c.b};
    }
    inline Color operator-(const Color &c) const {
        return {r - c.r, g - c.g, b - c.b};
    }
    inline Color operator*(const Float t) const {
        return {r * t, g * t, b * t};
    }
    inline auto operator/(const Float &t) const {
        return *this * (1 / t);
    }
    // The element wize vector multiplication I need for the scaling operation
    inline Color operator*(const Color &c) const {
        return {r * c.r, g * c.g, b * c.b};
    }
};

inline Color operator*(const Float t, const Color &c) {
    return c * t;
}

class Material {
public:
    Color indexOfRefraction;    // It is wavelength dependent
    Color reflectance;          // The surface color of the material
    Color transmission;         // The transmission of a trtansparent material
    Float surfaceSmoothness;    // 1 = perfectly flat (mirror), 0 = perfectly diffuse PAR@@?
    // For the crtscene format - controls whether flat or smooth shading is used for triangle meshes
    bool smoothShading = false;
    bool emitter = false;
    Color c;        // The color of the light that the object emits. (e.g. black body/heated metal)
    Float intensity;
};

#endif
