#ifndef JARTSY_CAMERA_H
#define JARTSY_CAMERA_H

#include "jartsyla.h"
#include "film.h"
#include "ray.h"

class Camera {
public:
    Transform pixel2world;  // Transform from film pixel coordinate system to world coordinate system
    Float tso, tsc;         // Time Shutter Open, Time Shutter Close. For motion blur if things are moving in the scene?
    Point pclens;           // The point (converted to world coordinates) whrere the center of the lens "aperture" sits.

    // These are not really needed after the construction of the camera. Just for debugging?
    const Transform transform; // Transform from camera coordinate system to world coordinate system
    const Film film;
    const Float focalLength; // In meters

    Camera(Transform tr, Film f, Float fl, Float so = 0, Float sc = 0)
        : tso(so), tsc(sc), transform(tr), film(f), focalLength(fl) {
        // Calculate the transformation from film/sensor pixel coordinates to world coordinates
        // There is a silent presumption of square pixels here
        Float aspect       = (Float)f.resolution.y / (Float)f.resolution.x;
        Float x            = sqrt(Sqr(film.diagonal) / (1 + Sqr(aspect)));
        Float y            = aspect * x;
        Vector scPixel2NDC = {(Float)1 / f.resolution.x, (Float)1 / f.resolution.y, 1};
        Vector scNDC2Film  = {x, -y, 1};
        Vector scPic2Film  = scPixel2NDC * scNDC2Film;
        // PAR@@@@ Same as above. I'll delete one when I debug it and am sure I'm not crazy :-)
        Vector scPix2Film = {x / f.resolution.x, -y / f.resolution.y, 1};
		// The camera points in the -Z direction with the XY plane of a right handed
		// coordinate system being the "film" plane
        Vector trNDC2Film = {-x / 2, y / 2, -fl};
        Transform Px2Film = {trNDC2Film, {}, scPix2Film};

        pixel2world = Px2Film;
        pixel2world.s = pixel2world.s * tr.s;
        pixel2world.t += tr.t;
        pixel2world.r = tr.r;

        pclens = tr({});    // The camera center in world coordinates
    }

    Ray GenerateRay(const Point2f pixelCoords, const Float t = 0) const {
        // Check if the pixel coordinates are within the film/sensor resolution here maybe?
        Point pfilm  = {pixelCoords.x, pixelCoords.y, 0};
        Point pworld = pixel2world(pfilm);
        return RayFromTo(pclens, pworld, Lerp(t, tso, tsc));
    };
};

#endif