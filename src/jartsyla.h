#ifndef JARTSY_LA_H
#define JARTSY_LA_H

#include "jartsy.h"

// All the linear algebra stuff, vectors, quaternions, transforms, points ...
// Stolen, borrowed, ideas from SolveSpace 

class Vector {
public:
    Float x, y, z;

    inline Vector operator-() const { return {-x, -y, -z}; }
    inline auto operator[](int i) {
        switch(i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: jartsyassert(false, "Unexpected vector element index");
        }
    }
    inline auto &operator+=(const Vector &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    inline auto &operator-=(const Vector &v) { return *this += -v; }
    inline auto &operator*=(const Float t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }
    inline auto &operator/=(const Float t) { return *this *= (1 / t); }

    inline bool operator==(const Vector &v) const { return x == v.x && y == v.y && z == v.z; }
    // Should this or the operator== be the exact comparison? Which one will I use more often?
    inline bool EqualsExactly(const Vector &v) const { return x == v.x && y == v.y && z == v.z; };

    inline Vector operator+(const Vector &v) const { return {x + v.x, y + v.y, z + v.z}; }
    inline Vector operator-(const Vector &v) const { return {x - v.x, y - v.y, z - v.z}; }
    inline Vector operator*(const Float t) const { return {x * t, y * t, z * t}; }
    inline auto operator/(const Float &t) const { return *this * (1 / t); }
    // The element wize vector multiplication I need ffor the scaling operation
    inline Vector operator*(const Vector &v) const { return {x * v.x, y * v.y, z * v.z}; }

    inline Vector Cross(const Vector &v) const {
        return {-(z * v.y) + (y * v.z), (z * v.x) - (x * v.z), -(y * v.x) + (x * v.y)};
    }
    inline auto Dot(const Vector &v) const { return (x * v.x + y * v.y + z * v.z); }

    inline auto MagSquared() const { return x * x + y * y + z * z; }
    inline auto Magnitude() const { return (Float)sqrt(x * x + y * y + z * z); }
    inline Vector WithMagnitude(Float t) const {
        Float m = Magnitude();
        if(EXACT(0 == m)) {
            // We can do a zero vector with zero magnitude, but not any other cases.
            if(fabs(t) > 1e-100) {
                dbp("Vector::WithMagnitude(%g) of zero vector!", t);
            }
            return {0, 0, 0};
        } else {
            return *this * (t / m);
        }
    }

/* Float DirectionCosineWith(Vector b) const;
    static Vector AtIntersectionOfPlanes(Vector n1, Float d1, Vector n2, Float d2);
    static Vector AtIntersectionOfLines(Vector a0, Vector a1, Vector b0, Vector b1, bool *skew,
                                        Float *pa = NULL, Float *pb = NULL);
    static Vector AtIntersectionOfPlaneAndLine(Vector n, Float d, Vector p0, Vector p1,
                                               bool *parallel);
    static Vector AtIntersectionOfPlanes(Vector na, Float da, Vector nb, Float db, Vector nc,
                                         Float dc, bool *parallel);
    static void ClosestPointBetweenLines(Vector pa, Vector da, Vector pb, Vector db, Float *ta,
                                         Float *tb);

    Vector Normal(int which) const;
    Vector RotatedAbout(Vector orig, Vector axis, Float theta) const;
    Vector RotatedAbout(Vector axis, Float theta) const;
    Vector DotInToCsys(Vector u, Vector v, Vector n) const;
    Vector ScaleOutOfCsys(Vector u, Vector v, Vector n) const;
    Float DistanceToLine(Vector p0, Vector dp) const;
    Float DistanceToPlane(Vector normal, Vector origin) const;
    bool OnLineSegment(Vector a, Vector b, Float tol = LENGTH_EPS) const;
    Vector ClosestPointOnLine(Vector p0, Vector deltal) const;
    Vector ProjectInto(hEntity wrkpl) const;
    Vector ProjectVectorInto(hEntity wrkpl) const;
    Float DivProjected(Vector delta) const;
    Vector ClosestOrtho() const;
    void MakeMaxMin(Vector *maxv, Vector *minv) const;
    Vector ClampWithin(Float minv, Float maxv) const;
    static bool BoundingBoxesDisjoint(Vector amax, Vector amin, Vector bmax, Vector bmin);
    static bool BoundingBoxIntersectsLine(Vector amax, Vector amin, Vector p0, Vector p1,
                                          bool asSegment);
    bool OutsideAndNotOn(Vector maxv, Vector minv) const;
    Vector InPerspective(Vector u, Vector v, Vector n, Vector origin, Float cameraTan) const;
    Point2d Project2d(Vector u, Vector v) const;
    Point2d ProjectXy() const;
    Vector4 Project4d() const;
*/
};

inline Vector operator*(const Float t, const Vector &v) {
    return v * t;
}


using Point = Vector;   // Meeh this is not quite nice but anyway :-)
using Normal = Vector;

class Quaternion {
public:
    // w + (v.x)*i + (v.y)*j + (v.z)*k
    Vector v;
    Float w = 1;    // Make the default quaternion identity {{0, 0, 0}, 1}

    inline Quaternion operator-() const { return {-v, -w}; }
    inline Quaternion &operator+=(const Quaternion &q) { v += q.v; w += q.w; return *this; }
    inline auto operator-=(const Quaternion &q) { return *this += -q; }
    inline auto operator*=(const Float t) { v *= t; w *= t; return *this; }
    inline auto operator/=(const Float t) { return *this *= (1 / t); }

    inline bool operator==(const Quaternion &q) const { return v == q.v && w == q.w; }
    // Should this or the operator== be the exact comparison? Which one will I use more often?
    inline bool EqualsExactly(const Quaternion &q) const { return v == q.v && w == q.w; };

    inline Quaternion operator+(const Quaternion &q) const { return {v + q.v, w + q.w}; }
    inline Quaternion operator-(const Quaternion &q) const { return {v - q.v, w - q.w}; }
    inline Quaternion operator*(const Float &t) const { return {v * t, w * t}; }
    inline auto operator/(const Float &t) const { return *this * (1 / t); }

    inline auto MagSquared() const { return v.MagSquared() + w * w; }
    inline auto Magnitude() const { return sqrt(MagSquared()); }
    inline Quaternion WithMagnitude(Float t) const {
        Float m = Magnitude();
        if(EXACT(0 == m)) {
            // We can do a zero Quaternion with zero magnitude, but not any other cases.
            if(fabs(t) > 1e-100) {
                dbp("Quaternion::WithMagnitude(%g) of zero Quaternion!", t);
            }
            return {{0, 0, 0}, 0};
        } else {
            return *this * (t / m);
        }
    }

    inline Vector RotationU() const {
        return {w * w + v.x * v.x - v.y * v.y - v.z * v.z,
                2 * w * v.z + 2 * v.x * v.y,
                2 * v.x * v.z - 2 * w * v.y};
    }

    inline Vector RotationV() const {
        return {2 * v.x * v.y - 2 * w * v.z,
                w * w - v.x * v.x + v.y * v.y - v.z * v.z,
                2 * w * v.x + 2 * v.y * v.z};
    }

    inline Vector RotationN() const {
        return {2 * w * v.y + 2 * v.x * v.z,
                2 * v.y * v.z - 2 * w * v.x,
                w * w - v.x * v.x - v.y * v.y + v.z * v.z};
    }

    inline Vector operator()(const Vector p) const {
        // Rotate a vector according to the quaternion
        // Express the vector in the new basis
        return RotationU() * p.x + RotationV() * p.y + RotationN() * p.z;
    }

    static Quaternion From(Vector axis, Float dtheta) {
        Float c = cos(dtheta / 2.), s = sin(dtheta / 2.);
        axis = axis.WithMagnitude(s);
        return {axis, c};
    }

    static Quaternion From(Vector u, Vector v) {
        Vector n = u.Cross(v);

        Quaternion q;
        Float s, tr = 1 + u.x + v.y + n.z;
        if(tr > 1e-4) {
            s    = 2. * sqrt(tr);
            q.w  = s / 4.;
            q.v.x = (v.z - n.y) / s;
            q.v.y = (n.x - u.z) / s;
            q.v.z = (u.y - v.x) / s;
        } else {
            if(u.x > v.y && u.x > n.z) {
                s    = 2. * sqrt(1 + u.x - v.y - n.z);
                q.w  = (v.z - n.y) / s;
                q.v.x = s / 4.;
                q.v.y = (u.y + v.x) / s;
                q.v.z = (n.x + u.z) / s;
            } else if(v.y > n.z) {
                s    = 2. * sqrt(1 - u.x + v.y - n.z);
                q.w  = (n.x - u.z) / s;
                q.v.x = (u.y + v.x) / s;
                q.v.y = s / 4;
                q.v.z = (v.z + n.y) / s;
            } else {
                s    = 2. * sqrt(1 - u.x - v.y + n.z);
                q.w  = (u.y - v.x) / s;
                q.v.x = (n.x + u.z) / s;
                q.v.y = (v.z + n.y) / s;
                q.v.z = s / 4.;
            }
        }

        return q.WithMagnitude(1);
    }

	// Quaternion from a 3x3 rotation matrix, rows u, v, n
    static Quaternion From(Vector u, Vector v, Vector n) {
        Quaternion q;
        Float s, tr = 1 + u.x + v.y + n.z;
        if(tr > 1e-4) {
            s     = 2. * sqrt(tr);
            q.w   = s / 4.;
            q.v.x = (v.z - n.y) / s;
            q.v.y = (n.x - u.z) / s;
            q.v.z = (u.y - v.x) / s;
        } else {
            if(u.x > v.y && u.x > n.z) {
                s     = 2. * sqrt(1 + u.x - v.y - n.z);
                q.w   = (v.z - n.y) / s;
                q.v.x = s / 4.;
                q.v.y = (u.y + v.x) / s;
                q.v.z = (n.x + u.z) / s;
            } else if(v.y > n.z) {
                s     = 2. * sqrt(1 - u.x + v.y - n.z);
                q.w   = (n.x - u.z) / s;
                q.v.x = (u.y + v.x) / s;
                q.v.y = s / 4.;
                q.v.z = (v.z + n.y) / s;
            } else {
                s     = 2. * sqrt(1 - u.x - v.y + n.z);
                q.w   = (u.y - v.x) / s;
                q.v.x = (n.x + u.z) / s;
                q.v.y = (v.z + n.y) / s;
                q.v.z = s / 4.;
            }
        }

        return q.WithMagnitude(1.);
    }
};

class Transform {
public:
    // Transform Private Members
    Vector t;       // Translate XYZ
    Quaternion r;   // Rotate
    Vector s = {1, 1, 1}; // Scale XYZ
    // Or maybe a 4x4 transformation matrix directly?

    inline Vector operator()(const Vector &v) const {
        // PAR@@@@@@ Sequence of operations?
        return r(v*s+t);
    }
    
    // PAR@@@ transform multiplication?
};


template<typename T>
class Vector2 {
public:
    T x, y;

    inline Vector2 operator-() const { return {-x, -y}; }
    inline auto operator[](int i) {
        switch(i) {
        case 0: return x;
        case 1: return y;
        default: jartsyassert(false, "Unexpected Vector2 element index");
        }
    }
    inline auto &operator+=(const Vector2 &v) { x += v.x; y += v.y; return *this; }
    inline auto &operator-=(const Vector2 &v) { return *this += -v; }
    inline auto &operator*=(const T t) { x *= t; y *= t; return *this; }
    inline auto &operator/=(const T t) { return *this *= (1 / t); }

    inline bool operator==(const Vector2 &v) const { return x == v.x && y == v.y; }
    // Should this or the operator== be the exact comparison? Which one will I use more often?
    inline bool EqualsExactly(const Vector2 &v) const { return x == v.x && y == v.y; };

    inline Vector2 operator+(const Vector2 &v) const { return { x + v.x, y + v.y}; }
    inline Vector2 operator-(const Vector2 &v) const { return { x - v.x, y - v.y}; }
    inline Vector2 operator*(const T &t) const { return { x * t, y * t }; }
    inline auto operator/(const T &t) const { return *this * (1 / t); }

    inline auto Dot(const Vector2 &v) const { return (x * v.x + y * v.y); }

    inline auto MagSquared() const { return x * x + y * y; }
    inline auto Magnitude() const { return sqrt(x * x + y * y); }
    inline Vector2 WithMagnitude(T t) const {
        T m = Magnitude();
        if(EXACT(0 == m)) {
            // We can do a zero Vector2 with zero magnitude, but not any other cases.
            if(fabs(t) > 1e-100) {
                dbp("Vector2::WithMagnitude(%g) of zero Vector2!", t);
            }
            return {0, 0};
        } else {
            return *this * (t / m);
        }
    }
};

using Vector2f = Vector2<Float>;
using Point2f = Vector2f;
using Point2i = Vector2<int>;
using Point2u  = Vector2<unsigned>;

// Linearly interpolates a vale t in [0,1] to a value in the range [a,b]
// Didn't know this was called a LERP :-)
// https://docs.unity3d.com/ScriptReference/Vector3.Lerp.html
// Should woork with all fo the above. Untested.
template<typename T>
inline T Lerp(Float t, T a, T b) {
    return (1 - t) * a + t * b;
}

#endif
